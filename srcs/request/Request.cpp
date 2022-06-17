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

	Request::Request(const Request& src) 
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
			_headers.push_back(HeaderField(header[0], header[1]));
		}
	}
	
	void	Request::_deepCopy(const Request& src)
	{
	}

	std::ostream	&operator<<(std::ostream& ostr, const ft::Request& request)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::fstream	bodyFile(request._bodyFileName);

		ostr << std::left;
		ostr << getDisplayHeader("Request", REQUEST_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "method : " << request._method << std::endl;
		ostr << std::setw(fieldSize) << "path : " << request._path << std::endl;
		ostr << std::setw(fieldSize) << "version : " << request._version << std::endl;

		ostr << getDisplayHeader("headers", REQUEST_SHSIZE) << std::endl;
		for (size_t i = 0; i < request._headers.size(); i++)
			ostr << std::setw(fieldSize) << request._headers[i].key << " : " << request._headers[i].value << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;

		ostr << getDisplayHeader("body", REQUEST_SHSIZE) << std::endl;
		ostr << "name : " << request._bodyFileName << std::endl;
		ostr << "=============================================================================" << std::endl;
		while (bodyFile.good())
		{
			std::getline(bodyFile, line);
			ostr << line << std::endl;
		}
		ostr << "=============================================================================" << std::endl;
		ostr << getDisplayFooter(REQUEST_SHSIZE) << std::endl;
		
		ostr << getDisplayFooter(REQUEST_HSIZE) << std::endl;
		return ostr;
	}
}