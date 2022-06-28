#include "Server.hpp"

namespace ft
{
			
	Server::Server()
	{
	}

	Server::~Server()
	{
		_sockt.close();
		for (size_t i = 0; i < _hosts.size(); i++)
			delete _hosts[i];
		for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it)
			delete it->second;
	}

	Server::Server(ServerSockt sockt) : _sockt(sockt)
	{
	}

	Server::Server(const Server& src) :	_sockt(src._sockt), _hosts(src._hosts), _clients(src._clients)
	{
	}

	Server	&Server::operator=(const Server& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Server::delClient(Client *client)
	{
		int fd;

		fd = client->getSocktFd();
		delete client;
		_clients.erase(fd);
	}

	int	Server::getSocktFd() const
	{
		return _sockt.fd;
	}

	std::string	Server::getPortNumber() const
	{
		return _sockt.getPort();
	}

	void	Server::_deepCopy(const Server& src)
	{
		_sockt = src._sockt;
		_hosts = src._hosts;
		_clients = src._clients;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Server& server)
	{
		ostr << std::left;
		ostr << getDisplayHeader("Server", SERVER_HSIZE) << std::endl;

		ostr << server._sockt << std::endl;

		ostr << getDisplayHeader("hosts", SERVER_SHSIZE) << std::endl;
		for (size_t i = 0; i < server._hosts.size(); i++)
			ostr << *server._hosts[i] << std::endl;
		ostr << getDisplayFooter(SERVER_SHSIZE) << std::endl;

		ostr << getDisplayHeader("clients", SERVER_SHSIZE) << std::endl;
		for (std::map<int, Client *>::const_iterator it = server._clients.begin(); it != server._clients.end(); ++it)
			ostr << it->second << std::endl;
		ostr << getDisplayFooter(SERVER_SHSIZE) << std::endl;

		ostr << getDisplayFooter(SERVER_HSIZE) << std::endl;
		return ostr;
	}
}