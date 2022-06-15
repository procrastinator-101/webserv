#include "Client.hpp"

namespace ft
{
	
	Client::Client() : _keepAlive(true)
	{
	}

	Client::~Client()
	{
		_sockt.close();
	}

	Client::Client(const Client& src) : _keepAlive(src._keepAlive), _sockt(src._sockt), _request(src._request), _response(src._response)
	{
	}

	Client	&Client::operator=(const Client& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Client::_deepCopy(const Client& src)
	{
		_keepAlive = src._keepAlive;
		_sockt = src._sockt;
		_request = src._request;
		_response = src._response;
	}
}