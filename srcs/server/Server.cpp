#include "Server.hpp"
#include <exception>
#include <sys/_select.h>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_size_t.h>
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

	static	bool is_num(std::string &str)
	{
		for (size_t i = 0; i < str.size(); i++)
		{
			if (str[i] < '0' || str[i] > '9')
				return false;
		}
		return true;
	}

	static void		threat_autoindex(std::stringstream& streamLine, std::string& token)
	{
		static int i = 0;

		if (i == 1)
			throw std::runtime_error("Server: multiple autoIndex");
		streamLine >> token;
		if (token == "on")
			_autoIndex = true;
		else if (token != "off")
			throw std::runtime_error("Server: autoindex is not valid");
		if (streamLine.good())
		{
			streamLine >> token;
			if (token != "#")
				throw std::runtime_error("Server: too many arguments for autoindex");
		}
		i = 1;
	}

	Server::Server(std::ifstream& configFile) : _names(), _root(NULL), _autoIndex(false), _methods(), _indexes(), _errorPages(), _locations()
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
			if (key == "#")
				continue ;
			if (key == "}")
			{
				inside_server = 0;
				break ;
			}
			// else if (key == "listen" && lineStream.good())
			// {
			// 	lineStream >> value;
			// 	if (lineStream.good())
			// 		throw std::runtime_error("Server: too many arguments for listen");
				
			// 	std::vector<std::string> tmp;
			// 	tmp = ft::split(value, ":");
				
			// 	if (tmp.size() != 2)
			// 		throw std::runtime_error("Server: listen: invalid add/port");

			// 	std::string		add(tmp[0]);
			// 	std::string		port(tmp[1]);

			// 	tmp.clear();
			// 	Sockt	sockt_tmp(add, port, Sockt::defaultBacklog);
			// 	_sockt = sockt_tmp;
			// }
			else if (key == "root" && lineStream.good())
			{
				lineStream >> value;
				if (value != "#")
					_root = value;
				else
					throw std::runtime_error("Server: root is not valid");
				if (lineStream.good())
				{
					lineStream >> value;
					if (value != "#")
						throw std::runtime_error("Server: too many arguments for root");
				}
			}
			else if (key == "index" && lineStream.good())
			{
				int size = _indexes.size();
				while (lineStream.good())
				{
					lineStream >> value;
					if (value == "#")
						break ;
					_indexes.insert(value);
				}
				if (_indexes.size() - size == 0)
					throw std::runtime_error("Server: index is not valid");
			}
			else if (key == "error_page" && lineStream.good())//invalid code
			{
				std::string code_str;
				lineStream >> code_str;
				if (code_str == "#")
					throw std::runtime_error("Server: error_page is not valid");
				if (lineStream.good() && is_num(code_str))
				{
					int code = ::atoi(code_str.c_str());
					try
					{
						HttpStatus::resolve(code);
					}
					catch (std::exception& e)
					{
						throw std::runtime_error("Server: error_page: invalid code");
					}
					lineStream >> value;
					_errorPages[code] = value;
				}
				else
					throw std::runtime_error("error_page: invalid code/page");
				if (lineStream.good())
				{
					lineStream >> value;
					if (value != "#")
						throw std::runtime_error("Server: too many arguments for error_page");
				}
			}
			else if (key == "methods" && lineStream.good())
			{
				int size = _methods.size();
				while (lineStream.good())
				{
					lineStream >> value;
					if (value == "#")
						break ;
					if (value == "GET" || value == "POST" || value == "DELETE")
						_methods.insert(value);
					else
						throw std::runtime_error("Server: invalid method");
				}
				if (_methods.size() - size == 0)
					throw std::runtime_error("Server: methods is not valid");
			}
			else if (key == "server_name" && lineStream.good())
			{
				int size = _names.size();
				while (lineStream.good())
				{
					lineStream >> value;
					if (value == "#")
						break ;
					_names.insert(value);
				}
				if (_names.size() - size == 0)
					throw std::runtime_error("Server: server_name is not valid");
			}
			else if (key == "autoindex" && lineStream.good())
				threat_autoindex(lineStream, value);
			else if (key == "location" && lineStream.good())
			{
				std::string path;
				lineStream >> path;
				if (path == "#")
					throw std::runtime_error("Server: location is not valid");
				if (lineStream.good())
				{
					lineStream >> value;
					if (value != "#")
						throw std::runtime_error("Server: too many arguments for location");
				}
				Location location_tmp(configFile, _root, _autoIndex, _indexes, _methods);
				_locations[path] = location_tmp;
			}
			else
				throw std::runtime_error("Server: config file is not valid");
		}
		if (inside_server == 1 || _locations.size() == 0)
			throw std::runtime_error("Server: config file is not valid");
		
	}

	Server::Server(const Server& src) :	_sockt(src._sockt), _names(src._names), _root(src._root), _autoIndex(src._autoIndex), _methods(src._methods),
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
		_autoIndex = src._autoIndex;
		_methods = src._methods;
		_indexes = src._indexes;
		_errorPages = src._errorPages;
		_locations = src._locations;
		_clients = src._clients;
	}
}