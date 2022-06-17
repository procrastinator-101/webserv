#include "Client.hpp"

namespace ft
{
	const size_t Client::bufferSize = 1024;

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

	bool	Client::handleRequest()
	{
		int		ret;
		int		end;
		char	*ptr;
		char	buffer[bufferSize + 1];

		ret = ::recv(_sockt.fd, buffer, bufferSize, 0);
		if (ret < 0)
			throw std::runtime_error("Client:: recv failed");
		buffer[ret] = 0;
		ptr = ::strstr(buffer, "\r\n\r\n");
		if (!ptr)
			_request._msg.append(buffer);
		else
		{
			end = ptr - buffer;
			_request._msg.append(buffer, end);
			_request._parseMessage();
			_request._msg.clear();
			if (!_request._body.is_open())
				_request._body.open(getRandomFileName());
			_request._body << (buffer + end + 4);
		}
		return false;
	}

	bool	Client::handleResponse(const Server& server)
	{
		(void)server;
		return false;
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