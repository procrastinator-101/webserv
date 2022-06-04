#include "Response.hpp"

namespace ft
{
	Response::Response() : _statusCode(200)
	{
	}

	Response::~Response()
	{
	}

	Response::Response(const Response& src) : _statusCode(src._statusCode), _msg(src._msg), _body(src._body), _headers(src._headers)
	{
	}

	Response	&Response::operator=(const Response& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Response::_deepCopy(const Response& src)
	{
		_msg = src._msg;
		_body = src._body;
		_headers = src._headers;
		_statusCode = src._statusCode;
	}
}