#include "Nginy.hpp"
#include <fstream>

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

	void	Nginy::serve()
	{
		std::pair<int, Action>	candidate;

		while (1)
		{
			candidate = _multiplexer.fetch();
			if (candidate.first == -1)
				continue ;
			for (size_t i = 0; i < _servers.size(); i++)
			{
				if (candidate.first == _servers[i]._sockt.fd)
				{
					if (candidate.second == aRead)
					{
						Client	client;

						client._sockt = _servers[i]._sockt.accept();
						_servers[i]._clients[client._sockt.fd] = client;
					}
						;//accept the new connection
					break ;
				}
			}
			for (size_t i = 0; i < _servers.size(); i++)
			{
				for (size_t j = 0; j < _servers[i]._clients.size(); j++)
				{
					if (candidate.first == _servers[j]._clients[j]._sockt.fd)
					{
						if (candidate.second == aRead)
							std::cout << "handle response" << std::endl;//_servers[i].handleResponse();
						else if (candidate.second == aWrite)
							std::cout << "handle request" << std::endl;;//_servers[i].handleRequest();
						break ;
					}
				}
			}
		}
	}
	
	void	Nginy::_parseConfigFile()
	{
		std::string			key;
		std::string			line;
		std::ifstream		configFile;
		std::stringstream	lineStream;

		configFile.open(_configFileName.c_str());
		if (!configFile.is_open())
			throw std::runtime_error("Could not open config file");

		while (configFile.good())
		{
			std::getline(configFile, line);
			if (line.empty())
				continue ;
			lineStream << line;
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