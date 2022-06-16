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

	std::ostream	&operator<<(std::ostream& ostr, const Client& client)
	{
		const int	fieldSize = 30;

		ostr << std::left << std::boolalpha;
		ostr << getDisplayHeader("Client", CLIENT_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "keepAlive : " << client._keepAlive << std::endl;

		ostr << client._sockt << std::endl;
		ostr << client._request << std::endl;
		ostr << client._response << std::endl;
		
		ostr << getDisplayFooter(CLIENT_HSIZE) << std::endl;
		return ostr;
	}
}