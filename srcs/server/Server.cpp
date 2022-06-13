#include "Server.hpp"

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