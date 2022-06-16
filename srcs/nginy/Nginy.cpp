#include "Nginy.hpp"
#include <fstream>
#include <sys/_types/_size_t.h>

namespace ft
{
	Nginy::Nginy()
	{
	}
	
	Nginy::~Nginy()
	{
	}

	Nginy::Nginy(const std::string& configFileName) : _configFileName(configFileName)
	{
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
			candidates = _multiplexer.fetch();
			if (candidates.empty())
				continue ;
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
				it = candidates.find(_servers[j]._clients[j]._sockt.fd);
				if (it == candidates.end())
					continue;
				if (it->second & aRead)
					std::cout << "handle response" << std::endl;
				else if (it->second & aWrite)
					std::cout << "handle request" << std::endl;
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
			{
				try
				{
					_servers.push_back(Server(configFile));
				}
				catch (std::exception& e)
				{
					throw std::runtime_error("Error: " + std::string(e.what()));
				}
			}
			else
			{
				throw std::runtime_error("Invalid config file");
			}
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
}