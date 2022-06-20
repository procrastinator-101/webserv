#include "Nginy.hpp"

namespace ft
{
	Nginy::Nginy()
	{
		//!!!!!!!! to remove
		ServerSockt	sockt("127.0.0.1", "8080");
		Server	*server = new Server(sockt);
		_servers.push_back(server);
	}
	
	Nginy::~Nginy()
	{
		for (size_t i = 0; i < _servers.size(); i++)
			delete _servers[i];
	}

	Nginy::Nginy(const std::string& configFileName) : _configFileName(configFileName)
	{
		_parseConfigFile();
	}

	Nginy::Nginy(const Nginy& src) : _configFileName(src._configFileName), _multiplexer(src._multiplexer), _servers(src._servers)
	{
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
			// std::cout << *this << std::endl;
			// std::cout << _multiplexer << std::endl;
			// std::cout << "fetching started" << std::endl;
			candidates = _multiplexer.fetch();
			// std::cout << "fetching ended" << std::endl;
			if (candidates.empty())
				continue ;
			// std::cout << "candidates" << std::endl;
			// for (std::map<int, int>::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
				// std::cout << it->first << " ";
			// std::cout << "candidatesEnd" << std::endl;
			// std::cout << std::endl;
			_acceptNewClients(candidates);
			_serveClients(candidates);
		}
	}

	void	Nginy::fetchConfiguration(const std::string& configFileName)
	{
		_configFileName = configFileName;
		_parseConfigFile();
	}
	
	void	Nginy::_initiateServers()
	{
		//stop or retrying waking it up ????
		for (size_t i = 0; i < _servers.size(); i++)
		{
			_servers[i]->_sockt.wakeUp();
			_multiplexer.add(_servers[i]->_sockt.fd, aRead);
			// std::cout << "servers[" << i << "].fd : " << _servers[i]->_sockt.fd << std::endl;
		}
	}

	void	Nginy::_serveClients(std::map<int, int>& candidates)
	{
		bool	isFinished;
		std::map<int, int>::iterator		it;
		std::map<int, Client *>::iterator	cit;

		for (size_t i = 0; i < _servers.size(); i++)
		{
			for (cit = _servers[i]->_clients.begin(); cit != _servers[i]->_clients.end(); ++cit)
			{
				it = candidates.find(cit->first);
				if (it == candidates.end())
					continue;
				if (it->second & aRead)
				{
					isFinished = cit->second->handleRequest();
					if (isFinished)
					{
						_multiplexer.del(cit->first, aRead);
						_multiplexer.add(cit->first, aWrite);
						std::cout << cit->second->_request << std::endl;
					}
				}
				else if (it->second & aWrite)
				{
					// std::cout << "handle response" << std::endl;
					if (isFinished)
					{
						_multiplexer.del(cit->first, aWrite);
						if (cit->second->keepAlive())
							_multiplexer.add(cit->first, aRead);
						else
						{
							delete cit->second;
							_servers[i]->_clients.erase(it->second);
						}
					}
				}
				else
					std::cout << "fd except" << std::endl;
			}
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
				
				// std::cout << "////////////////////////////////////////////////////" << std::endl;
				// std::cout << lineStream.str() << std::endl;
				// std::cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
				lineStream >> token;
				// std::cout << token << std::endl;
				if (token == "#")
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
		else
			throw std::runtime_error("Server:: invalid key {" + key + "}");
	}

	ServerSockt	Nginy::_fetchServerSockt(std::stringstream& lineStream)
	{
		std::string	value;
		std::vector<std::string> tmp;

		lineStream >> value;
		if (lineStream.good())
			throw std::runtime_error("Server:: too many arguments for listen");
		
		tmp = ft::split(value, ":");
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
		{
			// std::cout << "--fd : " << sockt.fd << std::endl;
			// std::cout << host << std::endl;
			_servers[i]->_hosts.push_back(&host);
		}
		else
		{
			target = new Server(sockt);
			try
			{
				_servers.push_back(target);
				// std::cout << "fd : " << sockt.fd << std::endl;
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
		_configFileName = src._configFileName;
		_multiplexer = src._multiplexer;
		_servers = src._servers;
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