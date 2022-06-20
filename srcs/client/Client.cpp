#include "Client.hpp"

namespace ft
{
	Client::Client()
	{
	}

	Client::~Client()
	{
		_sockt.close();
	}

	Client::Client(const Client& src) : _sockt(src._sockt), _request(src._request), _response(src._response)
	{
	}

	Client	&Client::operator=(const Client& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	bool	Client::handleRequest()
	{
		bool	ret;
		
		ret = _request.receive(_sockt.fd);
		if (!ret)
			return ret;
		host = _fetchTargetedHost();
		_response.build(host);
		_request.reset();
		return ret;
	}

	void	Client::_prepareResponse()
	{

	}

	bool	Client::handleResponse(const Server& server)
	{
		(void)server;
		return false;
	}

	bool	Client::keepAlive() const
	{
		return _request._keepAlive;
	}

	void	Client::_deepCopy(const Client& src)
	{
		_sockt = src._sockt;
		_request = src._request;
		_response = src._response;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Client& client)
	{
		ostr << std::left << std::boolalpha;
		ostr << getDisplayHeader("Client", CLIENT_HSIZE) << std::endl;

		ostr << client._sockt << std::endl;
		ostr << client._request << std::endl;
		ostr << client._response << std::endl;
		
		ostr << getDisplayFooter(CLIENT_HSIZE) << std::endl;
		return ostr;
	}
}