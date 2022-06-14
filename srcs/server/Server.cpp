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

	static	is_num(std::string &str)
	{
		for (int i = 0; i < str.size(); i++)
		{
			if (str[i] < '0' || str[i] > '9')
				return false;
		}
		return true;
	}

	Server::Server(std::ifstream& configFile) : _names(), _root(NULL), _methods(), _indexes(), _errorPages(), _clients(), _locations()
	{
		int					inside_server = 0;
		std::string			line;

		std::getline(configFile, line);
		std::stringstream	ss(line);
		std ::string		token;
		ss >> token;
		if (token != "{")
			throw std::runtime_error("Server: config file is not valid");

		while (std::getline(configFile, line))
		{
			inside_server = 1;
			if (line.empty())
				continue ;
			std::stringstream	lineStream(line);
			std::string			key;
			std::string			value;
			lineStream >> key;
			if (key[0] == '#')
				continue ;
			if (key == "}")
			{
				inside_server = 0;
				break ;
			}
			else if (key == "listen" && lineStream)
			{
// *****************************************************************************************
			}
			else if (key == "root" && lineStream)
			{
				lineStream >> value;
				if (value != "#")
					_root = value;
				else
					throw std::runtime_error("Server: root is not valid");
				if (lineStream)
				{
					lineStream >> value;
					if (value != "#")
						throw std::runtime_error("Server: too many arguments for root");
				}
			}
			else if (key == "index" && lineStream)
			{
				int size = _indexes.size();
				while (lineStream)
				{
					if (value == "#")
						break ;
					lineStream >> value;
					_indexes.insert(value);
				}
				if (_indexes.size() - size == 0)
					throw std::runtime_error("Server: index is not valid");
			}
			else if (key == "error_page" && lineStream)
			{
				std::string code_str;
				lineStream >> code_str;
				if (code_str == "#")
					throw std::runtime_error("Server: error_page is not valid");
				if (lineStream && is_num(code_str))
				{
					int code = std::stoi(code_str);
					lineStream >> value;
					_errorPages[code] = value;
				}
				else
					throw std::runtime_error("error_page: invalid code/page");
				if (lineStream)
				{
					lineStream >> value;
					if (value != "#")
						throw std::runtime_error("Server: too many arguments for error_page");
				}
			}
			else if (key == "methods")
			{
				int size = _methods.size();
				while (lineStream)
				{
					lineStream >> value;
					if (value == "#")
						break ;
					if (value == "GET" || value == "POST" value == "DELETE")
						_methods.insert(value);
					else
						throw std::runtime_error("Invalid method");
				}
				if (_methods.size() - size == 0)
					throw std::runtime_error("Server: methods is not valid");
			}
			else if (key == "server_name" && lineStream)
			{
				int size = _names.size();
				while (lineStream)
				{
					lineStream >> value;
					if (value == "#")
						break ;
					_names.insert(value);
				}
				if (_names.size() - size == 0)
					throw std::runtime_error("Server: server_name is not valid");
			}
		

			
			
			
		}
		if (inside_server == 1 || _locations.size() == 0)
			throw std::runtime_error("Server: config file is not valid");
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