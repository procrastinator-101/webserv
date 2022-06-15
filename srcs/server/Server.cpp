#include "Server.hpp"
#include <cstddef>

namespace ft
{
			
	Server::Server()
	{
	}

	Server::~Server()
	{
	}

	Server::Server(std::ifstream& configFile) : _names(), _root(""), _autoIndex(off), _methods(), _indexes(), _errorPages(), _locations()
	{
		bool				isClosed;
		std::string			key;
		std::string			line;
		std ::string		token;

		isClosed = false;
		std::getline(configFile, line);
		std::stringstream	ss(line);
		ss >> token;
		if (token != "{")
			throw std::runtime_error("Server: config file is not valid");
		while (std::getline(configFile, line))
		{
			if (line.empty())
				continue ;
			std::stringstream	lineStream(line);
			
			lineStream >> key;
			if (key == "#")
				continue ;
			if (key == "}")
			{
				isClosed = true;
				break ;
			}
			else if (key == "listen" && lineStream.good())
				_fetchSockt(lineStream);
			else if (key == "server_name" && lineStream.good())
				_fetchServerNames(lineStream);
			else if (key == "root" && lineStream.good())
				_fetchRoot(lineStream);
			else if (key == "autoindex" && lineStream.good())
				_fetchAutoIndex(lineStream);
			else if (key == "methods" && lineStream.good())
				_fetchMethods(lineStream);
			else if (key == "index" && lineStream.good())
				_fetchIndexes(lineStream);
			else if (key == "error_page" && lineStream.good())
				_fetchErrorPages(lineStream);
			else if (key == "location" && lineStream.good())
				_fetchLocation(lineStream, configFile);
			else
				throw std::runtime_error("Server: config file is not valid");
		}
		if (!isClosed || _locations.size() == 0)
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

	void	Server::_fetchSockt(std::stringstream& lineStream)
	{
		std::string	value;
		std::vector<std::string> tmp;

		lineStream >> value;
		if (lineStream.good())
			throw std::runtime_error("Server: too many arguments for listen");
		
		tmp = ft::split(value, ":");
		if (tmp.size() != 2)
			throw std::runtime_error("Server: listen: invalid add/port");
		_sockt = Sockt(tmp[0], tmp[1], Sockt::defaultBacklog);
	}

	void	Server::_fetchServerNames(std::stringstream& lineStream)
	{
		size_t	size;
		std::string	value;

		size = _names.size();
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

	void	Server::_fetchRoot(std::stringstream& lineStream)
	{
		std::string	value;

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

	void	Server::_fetchAutoIndex(std::stringstream& streamLine)
	{
		std::string	value;
		static int i = 0;//!!!!!!!! error

		if (i == 1)
			throw std::runtime_error("Server: multiple autoIndex");
		streamLine >> value;
		if (value == "on")
			_autoIndex = on;
		else if (value != "off")
			throw std::runtime_error("Server: autoindex is not valid");
		if (streamLine.good())
		{
			streamLine >> value;
			if (value != "#")
				throw std::runtime_error("Server: too many arguments for autoindex");
		}
		i = 1;
	}

	void	Server::_fetchMethods(std::stringstream& lineStream)
	{
		size_t	size;
		std::string	value;

		size = _methods.size();
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

	void	Server::_fetchIndexes(std::stringstream& lineStream)
	{
		size_t	size;
		std::string	value;

		size = _indexes.size();
		while (lineStream.good())
		{
			lineStream >> value;
			if (value == "#")
				break ;
			_indexes.insert(value);
		}
		if (_indexes.size() == size)
			throw std::runtime_error("Server: index is not valid");
	}

	void	Server::_fetchErrorPages(std::stringstream& lineStream)
	{
		int			code;
		std::string	value;
		std::string	code_str;

		lineStream >> code_str;
		if (code_str == "#")
			throw std::runtime_error("Server: error_page is not valid");
		if (lineStream.good() && isnumber(code_str))
		{
			code = ::atoi(code_str.c_str());
			try
			{
				HttpStatus::resolve(code);
			}
			catch (std::exception& e)
			{
				throw std::runtime_error("Server: error_page: invalid code");
			}
			lineStream >> value;
			//value == # ????
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

	void	Server::_fetchLocation(std::stringstream& lineStream, std::ifstream& configFile)
	{
		std::string path;
		std::string	value;
		
		lineStream >> path;
		if (path == "#")
			throw std::runtime_error("Server: location is not valid");
		if (lineStream.good())
		{
			lineStream >> value;
			if (value != "#")
				throw std::runtime_error("Server: too many arguments for location");
		}
		_locations[path] = Location(configFile, _root, _autoIndex, _indexes, _methods);
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