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

	std::ostream	&operator<<(std::ostream& ostr, const Response& response)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << getDisplayHeader("Response", RESPONSE_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "version : " << response._version << std::endl;
		ostr << std::setw(fieldSize) << "status : " << response._status.code << " : " << response._status.msg << std::endl;

		ostr << getDisplaySubHeader("headers") << std::endl;
		for (size_t i = 0; i < response._headers.size(); i++)
			ostr << std::setw(fieldSize) << response._headers[i].key << " : " << response._headers[i].value << std::endl;
		ostr << getDisplaySubFooter("headers") << std::endl;

		ostr << getDisplaySubHeader("body") << std::endl;
		ostr << response._body << std::endl;
		ostr << getDisplaySubFooter("body") << std::endl;
		
		ostr << getDisplayFooter(RESPONSE_HSIZE) << std::endl;
		return ostr;
	}
}