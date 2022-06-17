#include "Server.hpp"

namespace ft
{
			
	Server::Server()
	{
	}

	Server::~Server()
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

		ostr << getDisplaySubHeader("hosts") << std::endl;
		for (size_t i = 0; i < server._hosts.size(); i++)
			ostr << *server._hosts[i] << std::endl;
		ostr << getDisplaySubFooter("hosts") << std::endl;

		ostr << getDisplaySubHeader("clients") << std::endl;
		for (std::map<int, Client *>::const_iterator it = server._clients.begin(); it != server._clients.end(); ++it)
			ostr << it->second << std::endl;
		ostr << getDisplaySubFooter("clients") << std::endl;

		ostr << getDisplayFooter(SERVER_HSIZE) << std::endl;
		return ostr;
	}
}