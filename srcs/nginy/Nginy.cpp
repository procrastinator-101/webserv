#include "Nginy.hpp"

namespace ft
{
	Nginy::Nginy() :	_configFileName(), _multiplexer(), _servers()
	{
	}
	
	Nginy::~Nginy()
	{
		for (size_t i = 0; i < _servers.size(); i++)
			delete _servers[i];
	}

	Nginy::Nginy(const std::string& configFileName) :	_configFileName(configFileName), _multiplexer(), _servers()
	{
		if(_configFileName.substr(_configFileName.find_last_of(".")) == ".conf")
			_parseConfigFile();
		else
			throw std::runtime_error("Nginy:: invalid config file name");
	}

	Nginy::Nginy(const Nginy& src)
	{
		(void)src;
	}

	Nginy	&Nginy::operator=(const Nginy& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Nginy::up()
	{
		std::map<int, int>	candidates;
		
		_initiateServers();
		while (1)
		{
			_manageTimeouts();
			candidates = _multiplexer.fetch(MULTIPLEXING_TIMEOUT);
			if (candidates.empty())
				continue ;
			_acceptNewClients(candidates);
			_serveClients(candidates);
		}
	}

	void	Nginy::_manageTimeouts()
	{
		std::map<int, Client *>::iterator	it;

		for (size_t i = 0; i < _servers.size(); i++)
		{
			for (it = _servers[i]->_clients.begin(); it != _servers[i]->_clients.end(); ++it)
			{
				//checks and handles connection and cgi timeouts
				if (it->second->timeOut())
				{
					_multiplexer.del(it->first, aRead);
					_multiplexer.add(it->first, aWrite);
				}
			}
		}
	}

	void	Nginy::fetchConfiguration(const std::string& configFileName)
	{
		_configFileName = configFileName;
		if(_configFileName.substr(_configFileName.find_last_of(".")) == ".conf")
			_parseConfigFile();
		else
			throw std::runtime_error("Nginy:: invalid config file name");
	}
	
	void	Nginy::_initiateServers()
	{
		int	optionValue;

		optionValue = 1;
		signal(SIGPIPE, SIG_IGN);
		for (size_t i = 0; i < _servers.size(); i++)
		{
			_servers[i]->_sockt.open();
			_servers[i]->_sockt.setStatusFlag(O_NONBLOCK);
			_servers[i]->_sockt.setOption(SOL_SOCKET, SO_REUSEADDR, &optionValue);
			_servers[i]->_sockt.bind();
			_servers[i]->_sockt.listen();
			_multiplexer.add(_servers[i]->_sockt.fd, aRead);
		}
	}

	void	Nginy::_serveClients(std::map<int, int>& candidates)
	{
		Client	*client;
		std::map<int, int>::iterator		it;
		std::map<int, Client *>::iterator	cit;

		for (size_t i = 0; i < _servers.size(); i++)
		{
			for (cit = _servers[i]->_clients.begin(); cit != _servers[i]->_clients.end();)
			{
				client = cit->second;
				++cit;
				it = candidates.find(client->getSocktFd());
				if (it == candidates.end())
					continue;
				if (it->second & aRead)
					_manageRequest(*client, *_servers[i]);
				else if (it->second & aWrite)
					_manageResponse(*client, *_servers[i]);
			}
		}
	}

	void	Nginy::_manageRequest(Client& client, Server& server)
	{
		std::pair<bool, Transmission>	ret;

		ret = client.handleRequest(server);
		// std::cout << "isRequestFinished : " << ret.first << " | transmission : " << ret.second << std::endl;
		if (ret.first)
		{
			_multiplexer.del(client._sockt.fd, aRead);
			if (ret.second == tError)
				server.delClient(&client);
			else
				_multiplexer.add(client._sockt.fd, aWrite);
		}
	}

	void	Nginy::_manageResponse(Client& client, Server& server)
	{
		std::pair<bool, Transmission>	ret;

		ret = client.handleResponse();
		// std::cout << "isResponseFinished : " << ret.first << " | transmission : " << ret.second << std::endl;
		if (ret.first)
		{
			// std::cout << client._response << std::endl;
			_multiplexer.del(client._sockt.fd, aWrite);
			if (ret.second != tError && client.keepAlive())
			{
				client._response.reset();
				_multiplexer.add(client._sockt.fd, aRead);
			}
			else
				server.delClient(&client);
		}
	}

	void	Nginy::_acceptNewClients(std::map<int, int>& candidates)
	{
		std::map<int, int>::iterator	it;

		for (size_t i = 0; i < _servers.size(); i++)
		{
			it = candidates.find(_servers[i]->_sockt.fd);
			if (it == candidates.end())
				continue;
			if (it->second & aRead)
			{
				Client	*client = new Client;

				try 
				{
					client->_sockt = _servers[i]->_sockt.accept();
					_servers[i]->_clients[client->_sockt.fd] = client;
					_multiplexer.add(client->_sockt.fd, aRead);
				}
				catch (std::exception& e)
				{
					delete client;
					throw ;
				}
			}
		}
	}

	void	Nginy::_parseConfigFile()
	{
		std::string			key;
		std::string			line;
		std::ifstream		configFile;
		

		configFile.open(_configFileName.c_str());
		if (!configFile.is_open())
			throw std::runtime_error("Could not open config file");

		while (configFile.good())
		{
			std::getline(configFile, line);
			if (line.empty())
				continue ;
			std::stringstream	lineStream(line);
			lineStream >> key;
			if (key == "#")
				continue ;
			if (key == "server")
				_parseServerBlock(configFile);
			else
				throw std::runtime_error("Invalid config file");
		}
		if (_servers.empty())
			throw std::runtime_error("config file:: No server found");
		configFile.close();
	}
	
	void	Nginy::_parseServerBlock(std::ifstream& configFile)
	{
		Host		*host;
		bool		isClosed;
		ServerSockt	sockt;
		std::string	line;
		std::string	token;

		sockt.fd = 0;
		host = new Host;
		isClosed = false;
		try
		{
			std::getline(configFile, line);
			std::stringstream	ss(line);
			ss >> token;
			if (token != "{")
				throw std::runtime_error("Server:: unexpected token {" + token + "}");
			while (configFile.good())
			{
				std::getline(configFile, line);
				if (line.empty())
					continue ;
				std::stringstream	lineStream(line);
				
				lineStream >> token;
				if (lineStream.fail() || token == "#")
					continue ;
				if (token == "}")
				{
					isClosed = true;
					break ;
				}
				if (!lineStream.good())
				{
					std::cout << *host << std::endl;
					throw std::runtime_error("Server:: invalid configuration!!");
				}
				else if (token == "listen")
					sockt = _fetchServerSockt(lineStream);
				else
					_fetchHostValue(*host, configFile, lineStream, token);
			}
			if (!isClosed || !sockt.fd)
				throw std::runtime_error("Server:: invalid configuration");
			_addHost(sockt, *host);
		}
		catch (std::exception& e)
		{
			delete host;
			throw ;
		}
	}

	void	Nginy::_fetchHostValue(Host& host, std::ifstream& configFile, std::stringstream &lineStream, std::string& key)
	{
		if (key == "server_name")
			host.fetchNames(lineStream);
		else if (key == "root")
			host.fetchRoot(lineStream);
		else if (key == "autoindex")
			host.fetchAutoIndex(lineStream);
		else if (key == "methods")
			host.fetchMethods(lineStream);
		else if (key == "index")
			host.fetchIndexes(lineStream);
		else if (key == "error_page")
			host.fetchErrorPages(lineStream);
		else if (key == "location")
			host.fetchLocation(configFile, lineStream);
		else if (key == "body_size")
			host.fetchBodySize(lineStream);
		else
			throw std::runtime_error("Server:: invalid key {" + key + "}");
	}

	ServerSockt	Nginy::_fetchServerSockt(std::stringstream& lineStream)
	{
		std::string	value;
		std::string	check;
		std::vector<std::string> tmp;

		lineStream >> value;
		if (lineStream.good())
		{
			lineStream >> check;
			if (check.length() && check != "#")
				throw std::runtime_error("Server:: too many arguments for listen");
		}
		
		tmp = split(value, ":");
		if (tmp.size() != 2)
			throw std::runtime_error("Server:: listen: invalid add/port");
		return ServerSockt(tmp[0], tmp[1]);
	}

	void	Nginy::_addHost(ServerSockt& sockt, Host& host)
	{
		size_t	i;
		Server	*target;

		for (i = 0; i < _servers.size(); i++)
		{
			if (_servers[i]->_sockt == sockt)
				break ;
		}
		if (i < _servers.size())
			_servers[i]->_hosts.push_back(&host);
		else
		{
			target = new Server(sockt);
			try
			{
				_servers.push_back(target);
			}
			catch (std::exception& e)
			{
				delete target;
				throw ;
			}
			target->_hosts.push_back(&host);
		}
	}

	void	Nginy::_deepCopy(const Nginy& src)
	{
		(void)src;
	}

	std::ostream	&operator<<(std::ostream& ostr, const Nginy& nginy)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << getDisplayHeader("Nginy", NGINY_HSIZE) << std::endl;

		ostr << std::setw(fieldSize) << "configFileName : " << nginy._configFileName << std::endl;

		ostr << getDisplayHeader("servers", NGINY_SHSIZE) << std::endl;
		for (size_t i = 0; i < nginy._servers.size(); i++)
			ostr << *nginy._servers[i] << std::endl;
		ostr << getDisplayFooter(NGINY_SHSIZE) << std::endl;

		ostr << getDisplayFooter(NGINY_HSIZE) << std::endl;
		return ostr;
	}
}