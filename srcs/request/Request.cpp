#include "Request.hpp"
#include <string>

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
			ret = _parseBuffer(buffer, received);
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

	bool	Request::_parseBuffer(char *buffer, size_t size)
	{
		int		end;
		char	*ptr;

		//if the header is completely received
		if (_bodySize)
		{
			_body << buffer;
			_bodySize += size;
		}
		//the header is still not completely received
		else
		{
			ptr = ::strstr(buffer, "\r\n\r\n");
			if (!ptr)
				_msg.append(buffer);
			else
			{
				end = ptr - buffer;
				_msg.append(buffer, end);
				_parseMessage();
				_bodyFileName = std::string(NGINY_VAR_PATH) + "/" + getRandomFileName();
				_body.open(_bodyFileName.c_str(), std::ios_base::out);
				_body << (buffer + end + 4);
				_bodySize += size - end - 4;
			}
		}
		return _checkEndParse();
	}

	bool	Request::_checkEndParse()
	{
		if (_bodySize >= _contentLength)
			return true;
		// else if (_bodySize > _contentLength)//handle here or until response
		// 	throw std::runtime_error("Bad Request");
		return false;
	}

	void	Request::_parseMessage()
	{
		std::vector<std::string>	msgLines;

		//split message by \n
		msgLines = split(_msg, "\n");

		//parse the message
		_parseStartLine(msgLines);
		_parseHeaders(msgLines, 1);
	}

	void	Request::_parseStartLine(std::vector<std::string>& msgLines)
	{
		std::vector<std::string>	startLine;

		if (msgLines.empty())
			throw std::runtime_error("Bad Request");
		startLine = split(msgLines[0], " ");
		if (startLine.size() != 3)
			throw std::runtime_error("Bad Request");
		_method = startLine[0];
		_path = startLine[1];
		_version = startLine[2];
	}

	void	Request::_parseHeaders(std::vector<std::string>& msgLines, size_t offset)
	{
		size_t	i;
		std::vector<std::string>	header;

		for (i = offset; i < msgLines.size() && !msgLines[i].empty(); i++)
		{
			header = split(msgLines[i], ": ");
			if (header.size() != 2)
				throw std::runtime_error("Bad Request");
			_headers.insert(std::make_pair(header[0], header[1]));
		}
	}
	
	bool	Request::isValid()
	{
		bool	ret;

		ret = _isStartLineValid();
		if (!ret)
			return ret;
		ret = _areHeadersValid();
		if (!ret)
			return ret;
		return _isBodyValid();
	}

	bool	Request::_isStartLineValid() const
	{
		if (_method != "GET" && _method != "POST" && _method != "DELETE")
			return false;
		if (_version != "HTTP/1.1")
			return false;
		//path empty !!!!!!!!!
		return true;
	}

	bool	Request::_areHeadersValid() const
	{

	}

	bool	Request::_isBodyValid() const
	{

	}

	void	Request::_deepCopy(const Request& src)
	{
		(void)src;
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