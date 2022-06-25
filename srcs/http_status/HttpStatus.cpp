#include "HttpStatus.hpp"
#include <stdexcept>
#include <string>
#include <utility>

namespace ft
{
	std::map<int, const std::string>	HttpStatus::_library;

	HttpStatus::HttpStatus(int code) : code(code), msg(resolve(code))
	{
	}

	HttpStatus::HttpStatus(std::string msg) : code(resolve(msg)), msg(msg)
	{
	}

	HttpStatus::~HttpStatus()
	{
	}

	HttpStatus::HttpStatus(const HttpStatus& src) : code(src.code), msg(src.msg)
	{
	}

	HttpStatus	&HttpStatus::operator=(const HttpStatus& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	int	HttpStatus::resolve(const std::string msg)
	{
		if (_library.empty())
			_initialiseLibrary();
		
		std::map<int, const std::string>::const_iterator	it = _library.begin();
		std::map<int, const std::string>::const_iterator	ite = _library.end();

		while (it != ite)
		{
			if (it->second == msg)
				return it->first;
			++it;
		}
		throw std::invalid_argument("invalid status message");
	}

	std::string	HttpStatus::resolve(const int code)
	{
		if (_library.empty())
			_initialiseLibrary();
		
		std::map<int, const std::string>::const_iterator	it;

		it = _library.find(code);
		if (it == _library.end())
			throw std::invalid_argument("invalid status code");
		return it->second;
	}

	void	HttpStatus::_initialiseLibrary()
	{
		_setInformationalResponseCodes();
		_setSuccessCodes();
		_setRedirectionCodes();
		_setClientErrorCodes();
		_setServerErrorCodes();
	}

	void	HttpStatus::_setInformationalResponseCodes()
	{
		_library.insert(std::make_pair(100, "Continue"));
		_library.insert(std::make_pair(101, "Switching Protocols"));
		_library.insert(std::make_pair(102, "Processing"));
		_library.insert(std::make_pair(103, "Early Hints"));
	}

	void	HttpStatus::_setSuccessCodes()
	{
		_library.insert(std::make_pair(200, "OK"));
		_library.insert(std::make_pair(201, "Created"));
		_library.insert(std::make_pair(202, "Accepted"));
		_library.insert(std::make_pair(203, "Non-Authoritative Information"));
		_library.insert(std::make_pair(204, "No Content"));
		_library.insert(std::make_pair(205, "Reset Content"));
		_library.insert(std::make_pair(206, "Partial Content"));
		_library.insert(std::make_pair(207, "Multi-Status"));
		_library.insert(std::make_pair(208, "Already Reported"));
		_library.insert(std::make_pair(226, "IM Used"));
	}

	void	HttpStatus::_setRedirectionCodes()
	{
		_library.insert(std::make_pair(300, "Multiple Choices"));
		_library.insert(std::make_pair(301, "Moved Permanently"));
		_library.insert(std::make_pair(302, "Found"));
		_library.insert(std::make_pair(303, "See Other"));
		_library.insert(std::make_pair(304, "Not Modified"));
		_library.insert(std::make_pair(305, "Use Proxy"));
		_library.insert(std::make_pair(306, "Switch Proxy"));
		_library.insert(std::make_pair(307, "Temporary Redirect"));
		_library.insert(std::make_pair(308, "Permanent Redirect"));
	}

	void	HttpStatus::_setClientErrorCodes()
	{
		_library.insert(std::make_pair(400, "Bad Request"));
		_library.insert(std::make_pair(401, "Unauthorized"));
		_library.insert(std::make_pair(402, "Payment Required"));
		_library.insert(std::make_pair(403, "Forbidden"));
		_library.insert(std::make_pair(404, "Not Found"));
		_library.insert(std::make_pair(405, "Method Not Allowed"));
		_library.insert(std::make_pair(406, "Not Acceptable"));
		_library.insert(std::make_pair(407, "Proxy Authentication Required"));
		_library.insert(std::make_pair(408, "Request Timeout"));
		_library.insert(std::make_pair(409, "Conflict"));
		_library.insert(std::make_pair(410, "Gone"));
		_library.insert(std::make_pair(411, "Length Required"));
		_library.insert(std::make_pair(412, "Precondition Failed"));
		_library.insert(std::make_pair(413, "Payload Too Large"));
		_library.insert(std::make_pair(414, "URI Too Long"));
		_library.insert(std::make_pair(415, "Unsupported Media Type"));
		_library.insert(std::make_pair(416, "Range Not Satisfiable"));
		_library.insert(std::make_pair(417, "Expectation Failed"));
		_library.insert(std::make_pair(418, "I'm a teapot"));
		_library.insert(std::make_pair(421, "Misdirected Request"));
		_library.insert(std::make_pair(422, "Unprocessable Entity"));
		_library.insert(std::make_pair(423, "Locked"));
		_library.insert(std::make_pair(424, "Failed Dependency"));
		_library.insert(std::make_pair(425, "Too Early"));
		_library.insert(std::make_pair(426, "Upgrade Required"));
		_library.insert(std::make_pair(428, "Precondition Required"));
		_library.insert(std::make_pair(429, "Too Many Requests"));
		_library.insert(std::make_pair(431, "Request Header Fields Too Large"));
		_library.insert(std::make_pair(451, "Unavailable For Legal Reasons"));
	}

	void	HttpStatus::_setServerErrorCodes()
	{
		_library.insert(std::make_pair(500, "Internal Server Error"));
		_library.insert(std::make_pair(501, "Not Implemented"));
		_library.insert(std::make_pair(502, "Bad Gateway"));
		_library.insert(std::make_pair(503, "Service Unavailable"));
		_library.insert(std::make_pair(504, "Gateway Timeout"));
		_library.insert(std::make_pair(505, "HTTP Version Not Supported"));
		_library.insert(std::make_pair(506, "Variant Also Negotiates"));
		_library.insert(std::make_pair(507, "Insufficient Storage"));
		_library.insert(std::make_pair(508, "Loop Detected"));
		_library.insert(std::make_pair(510, "Not Extended"));
		_library.insert(std::make_pair(511, "Network Authentication Required"));
	}

	void	HttpStatus::_deepCopy(const HttpStatus& src)
	{
		code = src.code;
		msg = src.msg;
	}

}