#include "Nginy.hpp"
#include <exception>
#include <string>
#include <utility>

namespace ft
{
	Nginy::Nginy()
	{
		//!!!!!!!! to remove
		Server	*server = new Server;

		server->_sockt = ServerSockt("127.0.0.1", "8080");
		_servers.insert(std::make_pair(server->_sockt.fd, server));
	}
	
	Nginy::~Nginy()
	{
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
			std::cout << _multiplexer << std::endl;
			std::cout << "fetching started" << std::endl;
			candidates = _multiplexer.fetch();
			std::cout << "fetching ended" << std::endl;
			if (candidates.empty())
				continue ;
			std::cout << "candidates" << std::endl;
			for (std::map<int, int>::const_iterator it = candidates.begin(); it != candidates.end(); ++it)
				std::cout << it->first << " ";
			std::cout << "candidatesEnd" << std::endl;
			std::cout << std::endl;
			_acceptNewClients(candidates);
			_serveClients(candidates);
		}
	}
	
	void	Nginy::_initiateServers()
	{
		//stop or retrying waking it up ????
		for (std::map<int, Server *>::iterator sit = _servers.begin(); sit != _servers.end(); ++sit)
		{
			sit->second->_sockt.wakeUp();
			_multiplexer.add(sit->second->_sockt.fd, aRead);
			std::cout << "servers[" << sit->first << "].fd : " << sit->second->_sockt.fd << std::endl;
		}
	}

	void	Nginy::_serveClients(std::map<int, int>& candidates)
	{
		std::map<int, int>::iterator		it;
		std::map<int, Client *>::iterator	cit;
		std::map<int, Server *>::iterator	sit;

		for (sit = _servers.begin(); sit != _servers.end(); ++sit)
		{
			for (cit = sit->second->_clients.begin(); cit != sit->second->_clients.end(); ++cit)
			{
				it = candidates.find(cit->first);
				if (it == candidates.end())
					continue;
				if (it->second & aRead)
					std::cout << "handle request" << std::endl;
				else if (it->second & aWrite)
					std::cout << "handle response" << std::endl;
				else
					std::cout << "fd except" << std::endl;
			}
		}
	}

	void	Nginy::_acceptNewClients(std::map<int, int>& candidates)
	{
		std::map<int, int>::iterator	it;

		for (std::map<int, Server *>::iterator sit = _servers.begin(); sit != _servers.end(); ++sit)
		{
			it = candidates.find(sit->second->_sockt.fd);
			if (it == candidates.end())
				continue;
			if (it->second & aRead)
			{
				Client	*client = new Client;

				try 
				{
					client->_sockt = sit->second->_sockt.accept();
					sit->second->_clients[client->_sockt.fd] = client;
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
			throw std::runtime_error("config file: No server found");
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
				std::cout << token << std::endl;
				if (!lineStream.good())
					throw std::runtime_error("Server:: invalid configuration!!");
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
		Server	*target;
		std::map<int, Server *>::iterator sit;

		for (sit = _servers.begin(); sit != _servers.end(); ++sit)
		{
			if (sit->second->_sockt == sockt)
				break ;
		}
		if (sit == _servers.end())
		{
			target = new Server(sockt);
			try
			{
				_servers.insert(std::make_pair(sockt.fd, target));
			}
			catch (std::exception& e)
			{
				delete target;
				throw ;
			}
			target->_hosts.push_back(&host);
		}
		else
			sit->second->_hosts.push_back(&host);
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

		ostr << getDisplaySubHeader("servers") << std::endl;
		for (std::map<int, Server *>::const_iterator sit = nginy._servers.begin(); sit != nginy._servers.end(); ++sit)
			ostr << *sit->second << std::endl;
		ostr << getDisplaySubFooter("servers") << std::endl;

		ostr << getDisplayFooter(NGINY_HSIZE) << std::endl;
		return ostr;
	}
}