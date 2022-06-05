#include "Request.hpp"

namespace ft
{
	Request::Request()
	{
	}

	Request::~Request()
	{
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

	void	Request::_deepCopy(const Request& src)
	{
		_msg = src._msg;
		_method = src._method;
		_path = src._path;
		_version = src._version;
		_headers = src._headers;
		_body = src._body;
	}
}