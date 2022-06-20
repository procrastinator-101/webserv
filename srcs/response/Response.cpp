#include "Response.hpp"
#include <cstring>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <sys/_types/_size_t.h>

namespace ft
{
	Response::Response() : _sent(0), _contentLength(0)
	{
	}

	Response::~Response()
	{
	}

	Response::Response(const Response& src)
	{
		(void)src;
	}

	Response	&Response::operator=(const Response& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	bool	Response::send(int fd)
	{
		int		ret;
		int		len;
		int		left;
		int		size;
		char	buffer[BUFFER_SIZE];
		static std::fstream		logs("response_log", std::ios_base::out);

		std::cout << "///////////////////////////////// Response ///////////////////////////////////////" << std::endl;
		//header message still has bufferSize or more bytes to send
		if (_sent + BUFFER_SIZE <= _msg.length())
		{
			// std::cout << "msgLength = " << _msg.length() << std::endl;
			ret = ::send(fd, _msg.c_str() + _sent, BUFFER_SIZE, 0);
			if (logs.is_open())
				std::cout << "is open" << std::endl;
			logs << _msg.c_str() + _sent;
			std::cout << _msg.c_str() + _sent;
		}
		else
		{
			// std::cout << "++msgLength = " << _msg.length() << std::endl;
			logs << "ooooo";
			left = 0;
			if (_sent < _msg.length())
			{
				left = _msg.length() - _sent;
				memmove(buffer, _msg.c_str(), _msg.length() - _sent);
			}
			_body.read(buffer + left, BUFFER_SIZE - left);
			len = _body.gcount();
			size = left + len;
			ret = ::send(fd, buffer, size, 0);
			{
				std::string s(buffer, size);
				logs << s;
				std::cout << s;
			}
			if (ret > 0 && ret < size)
			{
				if (ret > left)
					len -= size - ret;
				_body.seekg(_body.tellg() - std::streampos(len));
			}
		}
		if (ret < 0)
			throw std::runtime_error("Response:: send failed");
		else if (!ret)
			return true;
		else
			_sent += ret;
		return _sent == _contentLength + _msg.length();
	}

	void	Response::build(const std::vector<Host *>& hosts, const Request& request)
	{
		const Host											*host;
		std::map<std::string, std::string>::const_iterator	hostName;

		if (request._bodySize > request._contentLength)
		{
			_buildBadRequestResponse();
			return ;
		}
		hostName = request._headers.find("Host");
		if (hostName == request._headers.end())
		{
			_buildBadRequestResponse();
			return ;
		}
		host = _fetchTargetedHost(hosts, hostName->second);
		_prepare(host, request);
		//temporary
		_bodyFileName = std::string(NGINY_INDEX_PATH) + "/index.html";
		_status = 200;
		//end temporary
		_contentLength = getFileSize(_bodyFileName);
		_version = request._version;
		_constructStatusLine();
		_constructHeaders(request);
		_constructBody(request);
	}

	void	Response::_buildBadRequestResponse()
	{
	}

	void	Response::_constructStatusLine()
	{
		std::stringstream	line;
		
		line << _version << " " << _status.code << " " << _status.msg;
		_msg = line.str();
		_msg.append("\r\n");
	}

	void	Response::_constructHeaders(const Request& request)
	{
		std::stringstream	line;

		(void)request;
		line << "Content-Length: " << _contentLength;
		_msg.append(line.str());
		_msg.append("\r\n\r\n");
	}

	void	Response::_constructBody(const Request& request)
	{
		_body.open(_bodyFileName, std::ios_base::in);
		(void)request;
	}

	const Host	*Response::_fetchTargetedHost(const std::vector<Host *>& hosts, const std::string& name)
	{
		for (size_t i = 0; i < hosts.size(); i++)
		{
			if (hosts[i]->hasName(name))
				return hosts[i];
		}
		return hosts[0];
	}

	void	Response::_prepare(const Host* host, const Request& request)
	{
		(void)host;
		(void)request;
	}
	void	Response::_handleGetMethod(const Host* host, const Request& request)
	{
		(void)host;
		(void)request;
	}
	void	Response::_handlePostMethod(const Host* host, const Request& request)
	{
		(void)host;
		(void)request;
	}
	void	Response::_handleDeleteMethod(const Host* host, const Request& request)
	{
		(void)host;
		(void)request;
	}

	void	Response::reset()
	{
		_sent = 0;
		_contentLength = 0;
		_msg.clear();
		_version.clear();
		_status = 200;
		_headers.clear();
		_bodyFileName.clear();
		_body.close();
	}

	void	Response::_deepCopy(const Response& src)
	{
		(void)src;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Response& response)
	{
		const int		fieldSize = 30;
		std::string		line;
		std::ifstream	bodyFile;

		ostr << std::left;
		ostr << getDisplayHeader("Response", RESPONSE_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "version : " << response._version << std::endl;
		ostr << std::setw(fieldSize) << "status : " << response._status.code << " : " << response._status.msg << std::endl;

		ostr << getDisplayHeader("headers", RESPONSE_SHSIZE) << std::endl;
		for (std::map<std::string, std::string>::const_iterator it = response._headers.begin(); it != response._headers.end(); ++it)
			ostr << std::setw(fieldSize) << it->first << " : " << it->second << std::endl;
		ostr << getDisplayFooter(RESPONSE_SHSIZE) << std::endl;

		ostr << getDisplayHeader("body", RESPONSE_SHSIZE) << std::endl;
		ostr << "name : " << response._bodyFileName << std::endl;
		ostr << "=============================================================================" << std::endl;
		bodyFile.open((response._bodyFileName.c_str()));
		while (bodyFile.good())
		{
			std::getline(bodyFile, line);
			ostr << line << std::endl;
		}
		bodyFile.close();
		ostr << "=============================================================================" << std::endl;
		ostr << getDisplayFooter(RESPONSE_SHSIZE) << std::endl;
		
		ostr << getDisplayFooter(RESPONSE_HSIZE) << std::endl;
		return ostr;
	}
}