#include "Request.hpp"
#include <exception>
#include <string>
#include <sys/_types/_size_t.h>
#include <sys/select.h>
#include <utility>
#include <vector>

namespace ft
{
	Request::Request() :	_isReceiving(false), _isInChunk(false), _chunkLen(0), _chunkSize(0), _isTrailerSet(false),
							_isTrailerReached(false), _notImplemented(false), _status(good), _isInBody(false),
							_bodySize(0), _buffer(), _isChunked(false), _keepAlive(true), _contentLength(0), _querry(),
							_trailerHeaders(), _cookies(), _method(), _path(), _version(), _headers(), _bodyFileName(), _body()
	{
	}

	Request::~Request()
	{
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

	std::pair<bool, Transmission>	Request::receive(int fd)
	{
		int		received;
		char	str[BUFFER_SIZE];
		std::pair<bool, Transmission>	ret;

		ret = std::make_pair(true, tSuccess);
		_updateRecvState();
		received = ::recv(fd, str, BUFFER_SIZE, 0);
		if (received > 0)
			ret.first = _parse(str, received);
		else
			ret = std::make_pair(true, tError);
		//end of receiving
		if (ret.first)
		{
			_isReceiving = false;
			if (_status == fatal)
				ret.second = tBadContent;
		}
		return ret;
	}

	bool	Request::timeOut() const
	{
		if (!_isReceiving)
			return false;
		if (getTimeStamp(_begin) >= RECV_TIMEOUT)
			return true;
		return false;
	}

	void	Request::_updateRecvState()
	{
		if (_isReceiving)
			return ;
		_isReceiving = true;
		gettimeofday(&_begin, 0);
	}

	bool	Request::isReceiving() const
	{
		return _isReceiving;
	}

	bool	Request::_parse(char *str, size_t size)
	{
		bool	ret;
		size_t	end;
		size_t	start;

		ret = false;
		// std::cout << "----isBody : " << _isInBody << " bodySize : " <<_bodySize << std::endl;
		if (_isInBody)
			ret = _fetchBody(str, size);
		else
		{
			_buffer.append(str, size);
			end = _buffer.find(HTTP_BLANKlINE);
			if (end != std::string::npos)
			{
				start = size - (_buffer.size() - (end + ::strlen(HTTP_BLANKlINE)));
				ret = _parseHead();
				if (ret)
					return ret;
				_buffer.clear();
				ret = _fetchBody(str + start, size - start);
			}
		}
		// std::cout << "++++isBody : " << _isInBody << " bodySize : " <<_bodySize << std::endl;
		return ret;
	}

	bool	Request::_fetchBody(char *str, size_t size)
	{
		// std::cout << "isBody : " << _isInBody << " bodySize : " <<_bodySize << std::endl;
		if (!_isInBody)
		{
			_bodyFileName = std::string(NGINY_VAR_DATA_PATH) + "/" + getRandomFileName("request");
			_body.open(_bodyFileName.c_str(), std::ios_base::out);
		}
		_isInBody = true;
		if (!_isChunked)
		{
			_body.write(str, size);
			_bodySize += size;
			return _endBody();
		}
		if (!size)
			return false;
		_buffer.append(str, size);
		if (_isTrailerReached)
			return _fetchTrailerPart();
		return _fetchChunkedBody();	
	}

	bool	Request::_fetchChunkedBody()
	{
		bool		ret;
		size_t		len;
		
		ret = false;
		while (!ret)
		{
			if (!_isInChunk)
			{
				if (_buffer.find(HTTP_NEWLINE) != std::string::npos)
					ret = _beginChunk();
				else
					return false;
			}
			else
			{
				if (_chunkLen == _chunkSize)
				{
					if (_buffer.size() < 2)
						return false;
					if (_buffer.substr(0, ::strlen(HTTP_NEWLINE)) != HTTP_NEWLINE)
						return _setStatus(fatal);
					_buffer.erase(0, ::strlen(HTTP_NEWLINE));
					if (!_chunkSize)
						return true;
					_resetChunk();
				}
				else if (_chunkLen + _buffer.size() < _chunkSize)
				{
					_body << _buffer;
					_bodySize += _buffer.size();
					_chunkLen += _buffer.size();
					_buffer.clear();
					return false;
				}
				else
				{
					len = _chunkSize - _chunkLen;
					_chunkLen += len;
					_bodySize += len;
					_body.write(_buffer.c_str(), len);
					_buffer.erase(0, len);
				}
			}
		}
		return ret;
	}

	bool	Request::_fetchTrailerPart()
	{
		bool		ret;
		size_t		pos;
		std::string	line;

		if (_buffer.empty())
			return false;
		pos = 0;
		while (pos != std::string::npos)
		{
			pos = _buffer.find(HTTP_NEWLINE);
			if (pos != std::string::npos)
			{
				line = strdtok(_buffer, HTTP_NEWLINE);
				if (line.empty())
					return true;
				ret = _setStatus(_setTrailerHeaders(line));
				if (ret)
					return true;
			}
		}
		return false;
	}

	bool	Request::_beginChunk()
	{
		std::string	line;
		std::string	token;

		_isInChunk = true;

		//fetch chunk line
		line = strdtok(_buffer, HTTP_NEWLINE);
		if (line.empty())
			return _setStatus(fatal);

		//fetch chunksize field
		token = strtok(line, HTTP_WHITE_SPACES);
		if (token.empty())
			return _setStatus(fatal);
		
		//fetch chunk size
		try
		{
			_chunkSize = hstoz(token);
		}
		catch (std::exception& e)
		{
			return _setStatus(fatal);
		}
		if (!_chunkSize && _isTrailerSet)
		{
			_isTrailerReached = true;
			return _fetchTrailerPart();
		}
		return false;
	}

	void	Request::_resetChunk()
	{
		_chunkLen = 0;
		_chunkSize = 0;
		_isInChunk = false;
	}

	bool	Request::_setStatus(Status status)
	{
		if (status != good)
			_status = status;
		if (status == fatal)
			return true;
		return false;
	}

	Request::Status	Request::_setTrailerHeaders(std::string& line)
	{
		std::string	token;
		
		token = strdtok(line, ":");
		if (_trailerHeaders.find(token) != _trailerHeaders.end())
			return _setHeader(token, trim(line, HTTP_WHITE_SPACES));
		return bad;
	}

	bool	Request::_endBody()
	{
		if (_bodySize < _contentLength)
			return false;
		if (_body.is_open())
			_body.close();
		if (_bodySize > _contentLength)
			_status = fatal;
		return true;
	}

	bool	Request::_parseHead()
	{
		bool	ret;
		std::vector<std::string>	msgLines;

		msgLines = split(_buffer, "\r\n");
		ret = _setStatus(_parseRequestLine(msgLines));
		std::cout << "ret(_parseStartLine) : " << ret << std::endl;
		if (ret)
			return ret;
		ret = _setStatus(_parseHeaders(msgLines, 1));
		std::cout << "ret(_parseHeaders) : " << ret << std::endl;
		return ret;
	}

	Request::Status	Request::_parseRequestLine(std::vector<std::string>& msgLines)
	{
		Request::Status				ret;
		std::vector<std::string>	requestLine;

		if (msgLines.empty() || msgLines[0].length() > MAX_REQUEST_LINE_LENGTH)
			return bad;
		requestLine = split(msgLines[0], " ");
		if (requestLine.size() != 3)
			return bad;
		
		ret = _parseMethod(requestLine[0]);
		if (ret != good)
			return ret;
		ret = _parseUri(requestLine[1]);
		// std::cout << "ret : " << ret << std::endl;
		if (ret != good)
			return ret;
		return _parseVersion(requestLine[2]);
	}

	Request::Status	Request::_parseMethod(std::string& method)
	{
		_method = method;

		//check if method is an http token or has an http whitespace
		for (size_t i = 0; i < _method.length(); i++)
		{
			if (!http::isTchar(_method[i]) || std::string(HTTP_WHITE_SPACES).find(_method[i]) != std::string::npos)
				return bad;
		}
		if (_method != "GET" && _method != "POST" && _method != "DELETE")
			return bad;
		return good;
	}

	Request::Status	Request::_parseUri(std::string& uri)
	{
		if (uri.find_first_of(HTTP_WHITE_SPACES) != std::string::npos)
			return bad;
		try
		{
			uri = _decodePercentUri(uri);
		}
		catch (std::exception& e)
		{
			return bad;
		}
		_path = strdtok(uri, "?");
		_querry = uri;
		return good;
	}

	std::string	Request::_decodePercentUri(std::string& uri)
	{
		size_t		pos;
		size_t		start;
		std::string	percent;
		std::string	ret;

		start = 0;
		while (1)
		{
			pos = uri.find('%', start);
			if (pos == std::string::npos)
			{
				ret.append(uri, start);
				return ret;
			}
			ret.append(uri, start, pos - start);
			percent = uri.substr(pos, 3);
			ret.push_back(decodePercent(percent));
			start = pos + 3;
		}
	}

	Request::Status	Request::_parseVersion(std::string& version)
	{
		std::string	name;
		std::string	major;
		std::string	minor;

		_version = version;
		// std::cout << "_version : " <<  _version << "|" << std::endl;
		name = strdtok(version, "/");
		if (name != "HTTP" && name != "%x48.54.54.50")
			return bad;
		major = strdtok(version, ".");
		if (major != "1")
			return bad;
		minor = version;
		if (minor != "1")
			return bad;
		return good;
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
			// std::cout << "key : " << key << " " << msgLines[i] << std::endl;
			tmp = _setHeader(key, trim(msgLines[i], HTTP_WHITE_SPACES));
			// std::cout << "tmp : " << tmp << std::endl;
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
		std::string	token;
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
			try
			{
				_contentLength = stoz(value);
			}
			catch (std::exception& e)
			{
				return fatal;
			}
		}

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
			for (size_t i = 0; i < tmp.size(); i++)
			{
				tmp[i] = trim(tmp[i], " ");
				if (tmp[i] != "chunked")
				{
					_notImplemented = true;
					ret = bad;
				}
				else
					_isChunked = true;
			}
		}

		//Trailer
		else if (key == "Trailer")
		{
			_isTrailerSet = true;
			tmp = split(value, ",");
			if (tmp.empty())
				ret = bad;
			for (size_t i = 0; i < tmp.size(); i++)
			{
				token = trim(tmp[i], HTTP_WHITE_SPACES);
				if (!token.empty())
					_trailerHeaders.insert(token);
				else
					ret = bad;
			}
		}

		//cookies
		else if (key == "Cookie")
		{
			_cookies.push_back(trim(value, " "));
			return ret;
		}

		_headers.insert(std::make_pair(key, value));
		return ret;
	}

	Request::Status	Request::_checkStartLine() const
	{
		if (_path.empty())
			return bad;
		return good;
	}

	Request::Status	Request::_checkHeaders() const
	{
		std::map<std::string, std::string>::const_iterator	it;

		it = _headers.find("Host");
		if (it == _headers.end())
			return bad;
		if (_headers.find("Transfer-Encoding") != _headers.end() && _headers.find("Content-Length") != _headers.end())
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

	void	Request::reset()
	{
		_isReceiving = false;

		_resetChunk();

		_isTrailerSet = false;
		_isTrailerReached = false;

		_notImplemented = false;
		_status = good;
		_isInBody = false;
		_bodySize = 0;
		_buffer.clear();

		_isChunked = false;
		_keepAlive = true;
		_contentLength = 0;
		_querry.clear();
		_trailerHeaders.clear();
		_cookies.clear();

		_method.clear();
		_path.clear();
		_version.clear();
		_headers.clear();
		_bodyFileName.clear();
		_body.close();
	}

	void	Request::_deepCopy(const Request& src)
	{
		(void)src;
	}

	Request::Status	Request::status() const
	{
		return _status;
	}

	std::ostream	&operator<<(std::ostream& ostr, const ft::Request& request)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::ifstream	bodyFile;

		ostr << std::left;
		ostr << getDisplayHeader("Request", REQUEST_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "_isTrailerSet : " << request._isTrailerSet << std::endl;
		ostr << std::setw(fieldSize) << "_isChunked : " << request._isChunked << std::endl;
		ostr << std::setw(fieldSize) << "keepAlive : " << request._keepAlive << std::endl;
		ostr << std::setw(fieldSize) << "state : " << request._status << std::endl;

		ostr << std::setw(fieldSize) << "method : " << request._method << std::endl;
		ostr << std::setw(fieldSize) << "path : " << request._path << std::endl;
		ostr << std::setw(fieldSize) << "querry : " << request._querry << std::endl;
		ostr << std::setw(fieldSize) << "version : " << request._version << std::endl;

		ostr << getDisplayHeader("TrailerHeaders", REQUEST_SHSIZE) << std::endl;
		for (std::set<std::string>::const_iterator it = request._trailerHeaders.begin(); it != request._trailerHeaders.end(); ++it)
			ostr << *it << " ";
		ostr << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("cookies", REQUEST_SHSIZE) << std::endl;
		for (std::vector<std::string>::const_iterator it = request._cookies.begin(); it != request._cookies.end(); ++it)
			ostr << *it << " ";
		ostr << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("headers", REQUEST_SHSIZE) << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("body", REQUEST_SHSIZE) << std::endl;
		ostr << "name : " << request._bodyFileName << std::endl;
		// ostr << "=============================================================================" << std::endl;
		// bodyFile.open((request._bodyFileName.c_str()));
		// while (bodyFile.good())
		// {
		// 	std::getline(bodyFile, line);
		// 	ostr << line << std::endl;
		// }
		// bodyFile.close();
		// ostr << "=============================================================================" << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;
		
		ostr << getDisplayFooter(REQUEST_HSIZE) << std::endl;
		return ostr;
	}
}