#include "Nginy.hpp"
#include <algorithm>

namespace ft
{
	Nginy::Nginy() :	_configFileName(), _env(0), _multiplexer(), _servers()
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

	Nginy::Nginy(const char **env) :	_configFileName(), _env(env), _multiplexer(), _servers()
	{
	}

	Nginy::Nginy(const std::string& configFileName, const char **env) :	_configFileName(configFileName), _env(env), _multiplexer(), _servers()
	{
		_parseConfigFile();
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
			candidates = _multiplexer.fetch(MULTIPLEXING_TIMEOUT);
			if (candidates.empty())
				continue ;
			_manageTimeouts();
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
				//check connection timeouts

				//check response timeouts : cgi timeouts
				if (it->second->_response.isTimedOut())
				{
					_servers[i]->delClient(it->second);
					_multiplexer.add(it->first, aAll);
				}
			}
		}
	}

	void	Nginy::fetchConfiguration(const std::string& configFileName)
	{
		_configFileName = configFileName;
		_parseConfigFile();
	}
	
	void	Nginy::_initiateServers()
	{
		int	optionValue;

		optionValue = 1;
		//stop or retrying waking it up ????
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
					_manageRequest(*cit->second, *_servers[i]);
				else if (it->second & aWrite)
					_manageResponse(*cit->second, *_servers[i]);
			}
		}
	}

	void	Nginy::_manageRequest(Client& client, Server& server)
	{
		bool	isFinished;

		isFinished = client.handleRequest(server);
		std::cout << "isRequestFinished : " << isFinished << std::endl;
		if (isFinished)
		{
			std::cout << client._request << std::endl;
			client._request.reset();
			_multiplexer.del(client._sockt.fd, aRead);
			_multiplexer.add(client._sockt.fd, aWrite);
		}
	}

	void	Nginy::_manageResponse(Client& client, Server& server)
	{
		bool	isFinished;

		isFinished = client.handleResponse();
		std::cout << "isResponseFinished : " << isFinished << std::endl;
		if (isFinished)
		{
			// std::cout << client._response << std::endl;
			_multiplexer.del(client._sockt.fd, aWrite);
			if (client.keepAlive())
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
				Client	*client = new Client(_env);

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
		else if (key == "body_size")
			host.fetchBodySize(lineStream);
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