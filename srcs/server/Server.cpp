#include "Server.hpp"
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_timeval.h>
#include <utility>

namespace ft
{
			
	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	Server::Server(std::ifstream& configFile)
	{
		if (configFile.eof())
			return;
	}

	Server::Server(const Server& src) :	_sockt(src._sockt), _names(src._names), _root(src._root), _methods(src._methods),
										_indexes(src._indexes), _errorPages(src._errorPages), _locations(src._locations),
										_clients(src._clients)
	{
	}

	Server	&Server::operator=(const Server& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	std::pair<Client*, action>	Server::select()
	{
		int		ret;
		int		nfds;
		fd_set	rawSet;
		fd_set	readfds;
		fd_set	writefds;
		timeval	timeout;
		std::map<int, Client>::iterator	it;

		//set rawSet
		rawSet = _getRawFdSet();

		//initiliase readfds and writefds and nfds
		nfds = 1024;//!!!!!!!
		readfds = rawSet;
		writefds = rawSet;

		//select
		ret = ::select(nfds, &readfds, &writefds, 0, &timeout);
		if (ret < 0)
			throw std::runtime_error("client selection failure");
		
		//detect if any client is ready to receive
		it = _clients.begin();
		while (it != _clients.end())
		{
			if (FD_ISSET(it->first, &rawSet))
				return std::make_pair(&it->second, aWrite);
			++it;
		}

		//detect if any client is ready to send
		it = _clients.begin();
		while (it != _clients.end())
		{
			if (FD_ISSET(it->first, &rawSet))
				return std::make_pair(&it->second, aRead);
			++it;
		}
		throw std::runtime_error("No client is ready");
	}

	fd_set	Server::_getRawFdSet() const
	{
		fd_set	ret;
		std::map<int, Client>::const_iterator	it;

		FD_ZERO(&ret);
		it = _clients.begin();
		while (it != _clients.end())
		{
			FD_SET(it->first, &ret);
			++it;
		}
		return ret;
	}

	void	Server::_deepCopy(const Server& src)
	{
		_sockt = src._sockt;
		_names = src._names;
		_root = src._root;
		_methods = src._methods;
		_indexes = src._indexes;
		_errorPages = src._errorPages;
		_locations = src._locations;
		_clients = src._clients;
	}
}