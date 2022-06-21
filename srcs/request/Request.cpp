#include "Request.hpp"
#include <string>
#include <utility>
#include <vector>

namespace ft
{
	Request::Request() : _keepAlive(false), _bodySize(0), _contentLength(0)
	{
	}

	Request::~Request()
	{
	}

	Request::Request(std::string& msg) :  _keepAlive(false), _bodySize(0), _contentLength(0), _msg(msg)
	{
		_parseMessage();
	}

	Request::Request(const Request& src)
	{
		(void)src;
	}

	Request	&Request::operator=(const Request& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	bool	Request::receive(int fd)
	{
		bool	ret;
		int		received;
		char	buffer[BUFFER_SIZE + 1];

		ret = true;
		received = ::recv(fd, buffer, BUFFER_SIZE, 0);
		if (received < 0)
			throw std::runtime_error("Request:: recv failed");
		if (received)
		{
			buffer[received] = 0;
			ret = _parse(buffer, received);
		}
		return ret;
	}

	void	Request::reset()
	{
		_keepAlive = false;
		_bodySize = 0;
		_contentLength = 0;
		_msg.clear();
		_method.clear();
		_path.clear();
		_version.clear();
		_headers.clear();
		_bodyFileName.clear();
		_body.close();
	}

	bool	Request::_parse(char *buffer, size_t size)
	{
		int		end;
		bool	ret;
		char	*ptr;

		if (_bodySize)
			_fillBody(buffer, size);
		else
		{
			ptr = ::strstr(buffer, "\r\n\r\n");
			if (!ptr)
				_msg.append(buffer);
			else
			{
				end = ptr - buffer;
				_msg.append(buffer, end);
				ret = _parseMessage();
				if (ret)
					return true;
				_fillBody(buffer + end + 4, size - end - 4);
			}
		}
		return _endParse();;
	}

	void	Request::_fillBody(char *buffer, size_t size)
	{
		//body still empty
		if (!_bodySize)
		{
			_bodyFileName = std::string(NGINY_VAR_PATH) + "/" + getRandomFileName();
			_body.open(_bodyFileName.c_str(), std::ios_base::out);
		}
		_body << buffer;
		_bodySize += size;
	}

	bool	Request::_endParse()
	{
		if (_bodySize < _contentLength)
			return false;
		if (_body.is_open())
			_body.close();
		return true;
	}

	Request::Status	Request::_parseMessage()
	{
		std::vector<std::string>	msgLines;

		//split message by \n
		msgLines = split(_msg, "\r\n");//!!

		//parse the message
		_parseStartLine(msgLines);
		_parseHeaders(msgLines, 1);
	}

	Request::Status	Request::_parseStartLine(std::vector<std::string>& msgLines)
	{
		std::vector<std::string>	startLine;

		if (msgLines.empty())
			return bad;
		startLine = splitWhiteSpaces(msgLines[0]);
		if (startLine.size() != 3)
			return bad;
		_method = startLine[0];
		_path = startLine[1];
		_version = startLine[2];
		return _checkStartLine();
	}

	Request::Status	Request::_parseHeaders(std::vector<std::string>& msgLines, size_t offset)
	{
		size_t	i;
		Status	ret;
		std::vector<std::string>	header;

		for (i = offset; i < msgLines.size() && !msgLines[i].empty(); i++)
		{
			header = split(msgLines[i], ':');
			if (header.size() != 2)
				throw std::runtime_error("Bad Request");
			ret = _setHeader(header[0], removeTrailingWhiteSpaces(header[1]));
			if (ret == bad)
				return ret;
		}
		return ret;
	}
	
	bool	Request::isValid()
	{
		bool	ret;

		ret = _checkStartLine();
		if (!ret)
			return ret;
		ret = _checkHeaders();
		if (!ret)
			return ret;
		return _checkBody();
	}

	Request::Status	Request::_setHeader(const std::string& key, const std::string& value)
	{
		Status	ret;
		std::vector<std::string>	tmp;
		std::map<std::string, std::string>::const_iterator	it;

		ret = good;

		//connection
		if (key == "Connection")
		{
			if (value == "keep-alive")
				_keepAlive = true;
			else if (value == "close")
				_keepAlive = false;
			else
				ret = bad;
		}

		//Content-Length
		else if (key == "Content-Length")
		{
			if (isnumber(it->second))
				ret = bad;
		}

		//Content-Type : get the file extension to know wheter to send it o cgi or not

		//Host
		else if (key == "Host")
		{
			it = _headers.find("Host");
			if (it != _headers.end())
				ret = bad;
			else
			{
				tmp = split(value, ':');
				if (tmp.size() > 2)
					ret = bad;
			}
		}
		
		//Transfer-Encoding
		else if (key == "Transfer-Encoding")
		{
			tmp = split(value, ',');
			if (tmp.size() != 1)
				ret = bad;
			else if (tmp[0] != "chunked")
				ret = bad;
			else
				_isChunked = true;
		}

		_headers.insert(std::make_pair(key, value));
		return ret;
	}

	Request::Status	Request::_checkStartLine() const
	{
		if (_method != "GET" && _method != "POST" && _method != "DELETE")
			return bad;
		if (_path.empty())
			return bad;
		if (_version != "HTTP/1.1")
			return bad;
		return good;
	}

	Request::Status	Request::_checkHeaders() const
	{
	}

	Request::Status	Request::_checkBody() const
	{

	}

	void	Request::_deepCopy(const Request& src)
	{
		(void)src;
	}

	Request::badRequest::badRequest() : _str("Bad Request")
	{
	}

	Request::badRequest::badRequest(const std::string& str) : _str("Bad Request:: " + str)
	{
	}

	const char	*Request::badRequest::what() const throw()
	{
		return _str.c_str();
	}

	std::ostream	&operator<<(std::ostream& ostr, const ft::Request& request)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::ifstream	bodyFile;

		ostr << std::left;
		ostr << getDisplayHeader("Request", REQUEST_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "keepAlive : " << request._keepAlive << std::endl;
		ostr << std::setw(fieldSize) << "method : " << request._method << std::endl;
		ostr << std::setw(fieldSize) << "path : " << request._path << std::endl;
		ostr << std::setw(fieldSize) << "version : " << request._version << std::endl;

		ostr << getDisplayHeader("headers", REQUEST_SHSIZE) << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("body", REQUEST_SHSIZE) << std::endl;
		ostr << "name : " << request._bodyFileName << std::endl;
		ostr << "=============================================================================" << std::endl;
		bodyFile.open((request._bodyFileName.c_str()));
		while (bodyFile.good())
		{
			std::getline(bodyFile, line);
			ostr << line << std::endl;
		}
		bodyFile.close();
		ostr << "=============================================================================" << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;
		
		ostr << getDisplayFooter(REQUEST_HSIZE) << std::endl;
		return ostr;
	}
}