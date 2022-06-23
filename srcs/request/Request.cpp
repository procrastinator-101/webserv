#include "Request.hpp"
#include <string>
#include <sys/_types/_size_t.h>

namespace ft
{
	Request::Request() : _status(good), _bodySize(0), _buffer(), _isChunked(false), _keepAlive(true), _contentLength(0)
	{
	}

	Request::~Request()
	{
	}

	Request::Request(std::string& msg) :  _status(good), _bodySize(0), _buffer(msg), _isChunked(false), _keepAlive(true), _contentLength(0)
	{
		_parseHead();
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
		char	str[BUFFER_SIZE + 1];

		ret = true;
		received = ::recv(fd, str, BUFFER_SIZE, 0);
		if (received < 0)
			throw std::runtime_error("Request:: recv failed");
		if (received)
		{
			str[received] = 0;
			ret = _parse(str, received);
		}
		return ret;
	}

	void	Request::reset()
	{
		_status = good;
		_bodySize = 0;
		_buffer.clear();

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

	bool	Request::_parse(char *str, size_t size)
	{
		bool	ret;
		size_t	end;
		size_t	start;

		if (_bodySize)
			ret = _fetchBody(str, size);
		else
		{
			_buffer.append(str);
			end = _buffer.find(HTTP_BLANKlINE);
			if (end != std::string::npos)
			{
				start = size - (_buffer.size() - (end + 4));
				ret = _parseHead();
				if (ret)
					return ret;
				_buffer.clear();
				ret = _fetchBody(str + start, size - start);
			}
		}
		return ret;
	}

	bool	Request::_fetchBody(char *str, size_t size)
	{
		bool	ret;
		char	*ptr;

		if (!size)
			return false;
		if (!_bodySize)
		{
			_bodyFileName = std::string(NGINY_VAR_PATH) + "/" + getRandomFileName();
			_body.open(_bodyFileName.c_str(), std::ios_base::out);
		}
		if (!_isChunked)
		{
			_body << str;
			_bodySize += size;
			return _endBody();
		}
		_buffer.append(str, size);
		if (_isTrailerReached)
			return _fetchTrailerPart();
		return _fetchChunkedBody();	
	}

	bool	Request::_fetchChunkedBody()
	{
		size_t		end;
		std::string	line;
		std::string	token;
		
		while (1)
		{
			end = _buffer.find(HTTP_NEWLINE);
			if (end != std::string::npos)
			{
				line = strdtok(_buffer, HTTP_NEWLINE);

				//end of transmission or end data chunk
				if (line.empty())
				{
					if (_isInChunk && _chunkLen == _chunkSize)
					{
						_chunkSize = 0;
						_chunkLen = 0;
						_isInChunk = false;
						continue ;
					}
					_status = fatal;
					return true;
				}

				//new Chunk initialisation
				if (!_isInChunk)
				{
					_isInChunk = true;
					token = strtok(line, WHITE_SPACES);
					if (token.empty())
					{
						_status = fatal;
						return true;
					}
					_chunkSize = ::atoi(token.c_str());//hex!!!!!!!!!! + check reasonnable chunsize
					if (!_chunkSize)
					{
						_isTrailerReached = true;
						return _fetchTrailerPart();
					}
				}
				//end of chunk data
				else
				{
					_body << line;
					_bodySize += line.size();
					_chunkLen += line.size();
					if (_chunkLen != _chunkSize)
					{
						_status = fatal;
						return true;
					}
					_chunkSize = 0;
					_chunkLen = 0;
					_isInChunk = false;
				}
			}
			else if (_isInChunk)
			{
				token.clear();
				token.push_back(*_buffer.rbegin());
				_buffer.resize(_buffer.size() - 1);
				_buffer.swap(token);
				_body << token;
				_bodySize += token.size();
				_chunkLen += token.size();
			}
			else
				break;
		}
	}

	bool	Request::_fetchTrailerPart()
	{
		Status		ret;
		size_t		pos;
		std::string	line;

		if (!_buffer.empty())
			return false;
		pos = 0;
		while (pos != std::string::npos)
		{
			pos = _buffer.find(HTTP_NEWLINE);
			if (pos != std::string::npos)
			{
				line = strdtok(line, HTTP_NEWLINE);
				if (line.empty())
					return true;
				ret = _setTrailerHeaders(line);
				if (ret != good)
				{
					_status = ret;
					if (ret == fatal)
						return true;
				}
			}
		}
		return false;
	}

	Request::Status	Request::_setTrailerHeaders(std::string& line)
	{
		Status		ret;
		std::string	token;

		token = strdtok(line, ":");
		if (_trailerHeaders.find(token) == _trailerHeaders.end())
		{
			ret = _setHeader(token, line);
			if (ret != fatal)
				return bad;
		}
		return good;
	}

	bool	Request::_endBody()
	{
		if (_bodySize < _contentLength)
			return false;
		if (_body.is_open())
			_body.close();
		return true;
	}

	Request::Status	Request::_parseHead()
	{
		Status	ret;
		Status	tmp;
		std::vector<std::string>	msgLines;

		msgLines = split(_buffer, "\r\n");
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
		startLine = mtsplit(msgLines[0], WHITE_SPACES);
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
			key = strdtok(msgLines[i], ":");
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
				tmp = split(value, ":");
				if (tmp.size() > 2)
					ret = bad;
			}
		}
		
		//Transfer-Encoding
		else if (key == "Transfer-Encoding")
		{
			tmp = split(value, ",");
			if (tmp.size() != 1)
				ret = bad;
			else if (tmp[0] != "chunked")//!!!!!!!!!
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