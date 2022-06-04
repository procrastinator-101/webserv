#include "Response.hpp"

namespace ft
{
	Response::Response()
	{
	}

	Response::~Response()
	{
	}

	Response::Response(const Response& src) : _msg(src._msg), _version(src._version), _status(src._status), _headers(src._headers), _body(src._body)
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
		_version = src._version;
		_status = src._status;
		_headers = src._headers;
		_body = src._body;
	}
}