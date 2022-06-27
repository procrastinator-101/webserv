#include "Client.hpp"

#include "../server/Server.hpp"

namespace ft
{
	Client::Client() :	_sockt(), _request(), _response()
	{
	}

	Client::~Client()
	{
		_sockt.close();
	}

	Client::Client(const char **env) :	_sockt(), _request(), _response(env)
	{
	}
	
	Client::Client(const Client& src)
	{
		(void)src;
	}

	Client	&Client::operator=(const Client& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	bool	Client::handleRequest(const Server& server)
	{
		bool	ret;
		
		ret = _request.receive(_sockt.fd);
		if (!ret)
			return ret;
		_response.build(server._hosts, _request);
		return ret;
	}

	void	Client::_prepareResponse()
	{
	}

	bool	Client::handleResponse()
	{
		return _response.send(_sockt.fd);
	}

	bool	Client::keepAlive() const
	{
		return _response._keepAlive;
	}

	void	Client::_deepCopy(const Client& src)
	{
		(void)src;
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