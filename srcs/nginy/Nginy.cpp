#include "Nginy.hpp"
#include <stdexcept>
#include <sys/_types/_fd_def.h>
#include <utility>

namespace ft
{
	Nginy::Nginy()
	{
	}
	
	Nginy::~Nginy()
	{
	}

	Nginy::Nginy(const std::string& configFile) : _configFile(configFile)
	{
	}

	Nginy::Nginy(const Nginy& src) : _configFileName(src._configFileName) ,_configFile(src._configFileName), _servers(src._servers)
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
		std::pair<Client*, action>	client;
		for (std::vector<Server>::size_type i = 0; i < _servers.size();)
		{
			client = _servers[i].select();
			// if (client.second == aWrite)
			// 	client.first->handleResponse();
			// else if ()
			// 	client.first->handleRequest();
			// else
				i++;
		}
	}
	

	void	Nginy::_parseConfigFile()
	{
		std::string	line;

		_configFile.open(_configFileName.c_str());
		if (!_configFile.is_open())
			throw std::runtime_error("Could not open config file");

		while (std::getline(_configFile, line))
		{
			if (line.empty())
				continue ;
			std::stringstream	lineStream(line);
			std::string			key;
			lineStream >> key;
			if (key[0] == '#')
				continue ;
			if (key == "server")
			{
				try
				{
					Server	server_tmp(_configFile);
					_servers.push_back(server_tmp);
				}
				catch (std::exception& e)
				{
					// std::cerr << "Error: " << e.what() << std::endl;
					throw std::runtime_error("Error: " + std::string(e.what()));
				}
			}
			else {
				throw std::runtime_error("Invalid config file");
			}
		}
		if (_servers.empty())
			throw std::runtime_error("config file: No server found");
		_configFile.close();
	}
	
	void	Nginy::_deepCopy(const Nginy& src)
	{
		_configFileName = src._configFileName;
		//kind of = op for ifstream
		_configFile.close();
		_configFile.open(_configFileName, src._configFile.flags());//flags
		_configFile.copyfmt(src._configFile);
		_servers = src._servers;
	}
}