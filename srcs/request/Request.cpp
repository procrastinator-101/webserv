#include "Request.hpp"
#include <string>
#include <utility>
#include <vector>

namespace ft
{
	Request::Request() : _status(good), _bodySize(0), _msg(), _isChunked(false), _keepAlive(true), _contentLength(0)
	{
	}

	Request::~Request()
	{
	}

	Request::Request(std::string& msg) :  _status(good), _bodySize(0), _msg(msg), _isChunked(false), _keepAlive(true), _contentLength(0)
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
		_status = good;
		_bodySize = 0;
		_msg.clear();

		_isChunked = false;
		_keepAlive = true;
		_contentLength = 0;

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
				_status = _parseMessage();
				_msg.clear();
				if (_status == fatal)
					return true;
				_fillBody(buffer + end + 4, size - end - 4);
			}
		}
		return _endParse();
	}

	bool	Request::_fillBody(char *buffer, size_t size)
	{
		bool	ret;
		char	*ptr;

		if (!_bodySize)
		{
			_bodyFileName = std::string(NGINY_VAR_PATH) + "/" + getRandomFileName();
			_body.open(_bodyFileName.c_str(), std::ios_base::out);
		}
		if (!_isChunked)
		{
			_body << buffer;
			_bodySize += size;
		}
		else
		{
			if (!_isTrailerReached)
				ret = _fillChunkedBody(buffer, size);
			if (_isTrailerReached)
			{
				ptr = ::strstr(buffer, "\r\n\r\n");
				if (!ptr)
					_msg.append(buffer);
				else
				{
					_msg.append(buffer, ptr - buffer);
					return true;
				}
			}
		}
		return ret;
	}

	bool	Request::_fillChunkedBody(char* &buffer, size_t size)
	{
		char	*ptr;
		std::string	token;
		
		while (1)
		{
			
			if (_chunkLen == _chunkSize)
			{
				_chunkLen = 0;
				token = strtok(lines[i]);//check empty token
				_chunkSize = ::atoi(token.c_str());
				if (!_chunkSize)
					return true;//++trailers
			}
			else
			{
				_body << lines[i];
				_bodySize += lines[i].size();
			}
		}
		if (!lines.empty())
		{
			if (lines.back() == "\r\n")
				return true;
			else if ()
		}
		
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
		Status	ret;
		Status	tmp;
		std::vector<std::string>	msgLines;

		msgLines = split(_msg, "\r\n");
		ret = _parseStartLine(msgLines);
		if (ret == fatal)
			return ret;
		tmp = _parseHeaders(msgLines, 1);
		if (tmp == good)
			return ret;
		return tmp;
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
		size_t		i;
		Status		ret;
		Status		tmp;
		std::string	key;

		ret = good;
		for (i = offset; i < msgLines.size() && !msgLines[i].empty(); i++)
		{
			key = strtok(msgLines[i], ':');
			tmp = _setHeader(key, removeTrailingWhiteSpaces(msgLines[i]));
			if (tmp == fatal)
				return fatal;
			else if (ret == good)
				ret = tmp;
		}
		if (ret != good)
			return ret;
		return _checkHeaders();
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
		std::map<std::string, std::string>::const_iterator	it;

		it = _headers.find("Host");
		if (it == _headers.end())
			return bad;
		return good;
	}

	Request::Status	Request::_checkBody() const
	{
		return good;
	}

	bool	Request::_isTrailerAllowedHeader(std::string& header) const
	{
		static std::set<std::string>	disallowedHeaders;

		if (disallowedHeaders.empty())
			_fillTrailerDisallowedHeaders(disallowedHeaders);
		return disallowedHeaders.find(header) == disallowedHeaders.end();
	}

	void	Request::_fillTrailerDisallowedHeaders(std::set<std::string>& disallowedHeaders) const
	{
		//message framing headers
		disallowedHeaders.insert("Transfer-Encoding");
		disallowedHeaders.insert("Content-Length");

		//routing
		disallowedHeaders.insert("Host");

		//request modifiers
			//controls
		disallowedHeaders.insert("Cache-Control");
		disallowedHeaders.insert("Expect");
		disallowedHeaders.insert("Max-Forwards");
		disallowedHeaders.insert("Pragma");
		disallowedHeaders.insert("Range");
		disallowedHeaders.insert("TE");

		//authentication headers
		disallowedHeaders.insert("Authorization");
		disallowedHeaders.insert("Set-Cookie");

		//response control data
		disallowedHeaders.insert("Age");
		disallowedHeaders.insert("Cache-Control");
		disallowedHeaders.insert("Expires");
		disallowedHeaders.insert("Date");
		disallowedHeaders.insert("Location");
		disallowedHeaders.insert("Retry-After");
		disallowedHeaders.insert("Vary");
		disallowedHeaders.insert("Warning");

		//headers determining how to process the payload
		disallowedHeaders.insert("Content-Encoding");
		disallowedHeaders.insert("Content-Type");
		disallowedHeaders.insert("Content-Range");
		disallowedHeaders.insert("Trailer");
	}

	void	Request::_deepCopy(const Request& src)
	{
		(void)src;
	}

	Request::badRequest::badRequest() : _str("Bad Request")
	{
	}

	Request::badRequest::~badRequest() throw()
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