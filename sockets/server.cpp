#include "Server.hpp"

namespace ft
{
			
	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	Server::Server(const Server& src) : _sockt(src._sockt), _name(src._name), _root(src._root), _aliases(src._aliases), _locations(src._locations)
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
		_name = src._name;
		_root = src._root;
		_aliases = src._aliases;
		_locations = src._locations;
	}
}