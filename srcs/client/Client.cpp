#include "Client.hpp"

#include "../server/Server.hpp"
#include <utility>

namespace ft
{
	Client::Client() :	_sockt(), _request(), _response()
	{
	}

	Client::~Client()
	{
		_sockt.close();
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

	std::pair<bool, Transmission>	Client::handleRequest(const Server& server)
	{
		std::pair<bool, Transmission>	ret;
		
		ret = _request.receive(_sockt.fd);
		if (!ret.first || ret.second == tError)
			return ret;

		// std::cout << "bodySize : " << _request._bodySize << "   fileSize : " << getFileSize(_request._bodyFileName) << std::endl;
		std::cout << _request << std::endl;

		//set up cgi
		_response._cgi.setServer(&server);
		_response._cgi.setClient(this);

		_response.build(server._hosts, _request);

		_request.reset();
		return ret;
	}

	bool	Client::timeOut()
	{
		//check connection timeout
		if (_request.timeOut())
		{
			std::cout << "request timeout" << std::endl;
			_request.reset();
			_response._constructErrorResponse(408);
			return true;
		}
		//check cgi timeout
		if (_response.timeOut())
		{
			std::cout << "response timeout" << std::endl;
			_response.reset();
			_response._constructErrorResponse(504);
			return true;
		}
		return false;
	}

	std::pair<bool, Transmission>	Client::handleResponse()
	{
		if (_response.isFinished())
			return _response.send(_sockt.fd);
		return std::make_pair(false, tSuccess);
	}

	bool	Client::keepAlive() const
	{
		return _response._keepAlive;
	}

	int		Client::getSocktFd() const
	{
		return _sockt.fd;
	}

	std::string	Client::getIpAddress() const
	{
		return _sockt.getIpAddress();
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