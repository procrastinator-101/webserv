#include "Nginy.hpp"
#include <fstream>
#include <sys/_types/_size_t.h>

namespace ft
{
	Nginy::Nginy()
	{
		//!!!!!!!! to remove
		Server	server;

		server._sockt = ServerSockt("127.0.0.1", "8080");
		// server._sockt.wakeUp();
		// Sockt	sockt = server._sockt.accept();

		// char host[NI_MAXHOST];
		// char service[NI_MAXSERV];

		// getnameinfo(reinterpret_cast<sockaddr*>(&sockt.address), sockt.addressLen, host, NI_MAXHOST, service, NI_MAXSERV, 0);
		// std::cout << "communication initiated with : " << host << " at service : " << service << std::endl;
		_servers.push_back(server);
		// std::cout << "constructor ok" << std::endl;
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
		for (size_t i = 0; i < _servers.size(); i++)
		{
			_servers[i]._sockt.wakeUp();
			_multiplexer.add(_servers[i]._sockt.fd, aRead);
			std::cout << "servers[" << i << "].fd : " << _servers[i]._sockt.fd << std::endl;
		}

	}

	void	Nginy::_serveClients(std::map<int, int>& candidates)
	{
		std::map<int, int>::iterator	it;

		for (size_t i = 0; i < _servers.size(); i++)
		{
			for (size_t j = 0; j < _servers[i]._clients.size(); j++)
			{
				it = candidates.find(_servers[i]._clients[j]._sockt.fd);
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

		for (size_t i = 0; i < _servers.size(); i++)
		{
			it = candidates.find(_servers[i]._sockt.fd);
			if (it == candidates.end())
				continue;
			if (it->second & aRead)
			{
				Client	client;

				client._sockt = _servers[i]._sockt.accept();
				_servers[i]._clients[client._sockt.fd] = client;
				_multiplexer.add(client._sockt.fd, aRead);
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
				_servers.push_back(Server(configFile));
			else
				throw std::runtime_error("Invalid config file");
		}
		if (_servers.empty())
			throw std::runtime_error("config file: No server found");
		configFile.close();
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
		for (size_t i = 0; i < nginy._servers.size(); i++)
			ostr << nginy._servers[i] << std::endl;

		ostr << getDisplayFooter(NGINY_HSIZE) << std::endl;
		return ostr;
	}
}