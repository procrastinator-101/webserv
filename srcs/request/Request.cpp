#include "Request.hpp"

namespace ft
{
	Request::Request()
	{
	}

	Request::~Request()
	{
	}

	Request::Request(std::string& msg) : _msg(msg)
	{
		_parseMessage();
	}

	Request::Request(const Request& src) : _msg(src._msg), _method(src._method), _path(src._body), _version(src._version), _headers(src._headers), _body(src._body)
	{
	}

	Request	&Request::operator=(const Request& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Request::_parseMessage()
	{
		size_t	offset;
		std::vector<std::string>	msgLines;

		//split message by \n
		msgLines = split(_msg, "\n");

		//parse the message
		offset = _parseStartLine(msgLines);
		offset = _parseHeaders(msgLines, 1);
		_parseBody(msgLines, offset);
	}

	size_t	Request::_parseStartLine(std::vector<std::string>& msgLines)
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
		return 1;
	}

	size_t	Request::_parseHeaders(std::vector<std::string>& msgLines, size_t offset)
	{
		size_t	i;
		std::vector<std::string>	header;

		for (i = offset; i < msgLines.size() && !msgLines[i].empty(); i++)
		{
			header = split(msgLines[i], ": ");
			if (header.size() != 2)
				throw std::runtime_error("Bad Request");
			_headers.push_back(HeaderField(header[0], header[1]));
		}
		return std::min(i + 1, msgLines.size());
	}
	
	size_t	Request::_parseBody(std::vector<std::string>& msgLines, size_t offset)
	{
		size_t	i;

		for (i = offset; i < msgLines.size(); i++)
		{
			_body.append(msgLines[i]);
			if (i < msgLines.size() - 1)
				_body.append("\n");
		}
		return i;
	}
	
	void	Request::_deepCopy(const Request& src)
	{
		_msg = src._msg;
		_method = src._method;
		_path = src._path;
		_version = src._version;
		_headers = src._headers;
		_body = src._body;
	}

	std::ostream	&operator<<(std::ostream& ostr, const ft::Request& request)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << getDisplayHeader("Server", SERVER_HSIZE) << std::endl;

		ostr << getDisplaySubHeader("start line");
		ostr << std::setw(fieldSize) << "method : " << request._method << std::endl;
		ostr << std::setw(fieldSize) << "path : " << request._path << std::endl;
		ostr << std::setw(fieldSize) << "version : " << request._version << std::endl;
		ostr << getDisplaySubFooter("start line");

		ostr << std::setw(fieldSize) << "root : " << server._root << std::endl;
		ostr << std::setw(fieldSize) << "autoIndex : " << server._autoIndex << std::endl;

		ostr << getDisplaySubHeader("methods");
		for (std::set<std::string>::const_iterator it = server._methods.begin(); it != server._methods.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("methods");

		ostr << getDisplaySubHeader("indexes");
		for (std::set<std::string>::const_iterator it = server._indexes.begin(); it != server._indexes.end(); ++it)
			ostr << *it << std::endl;
		ostr << getDisplaySubFooter("indexes");

		ostr << getDisplaySubHeader("errorPages");
		for (std::map<int, std::string>::const_iterator it = server._errorPages.begin(); it != server._errorPages.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplaySubFooter("errorPages");

		for (std::map<std::string, Location>::const_iterator it = server._locations.begin(); it != server._locations.end(); ++it)
			ostr << it->second << std::endl;
		
		for (std::map<int, Client>::const_iterator it = server._clients.begin(); it != server._clients.end(); ++it)
			ostr << it->second << std::endl;
		
		ostr << getDisplayFooter(SERVER_HSIZE) << std::endl;
		return ostr;
		
		ostr << "-- start line ----------------" << std::endl;
		ostr << std::setw(fieldSize) << "method : " << request._method << std::endl;
		ostr << std::setw(fieldSize) << "path : " << request._path << std::endl;
		ostr << std::setw(fieldSize) << "version : " << request._version << std::endl;
		ostr << "-- headers--------------------" << std::endl;
		for (size_t i = 0; i < request._headers.size(); i++)
			ostr << std::setw(fieldSize) << request._headers[i].key << " : " << request._headers[i].value << std::endl;
		ostr << "-- body ----------------------" << std::endl;
		ostr << request._body << std::endl;
		ostr << "________________________________________________________________" << std::endl;
		return ostr;
	}
}