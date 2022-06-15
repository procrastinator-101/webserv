#include "Nginy.hpp"
#include <stdexcept>
#include <sys/_types/_fd_def.h>
#include <sys/_types/_size_t.h>
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
		std::pair<int, Action>	candidate;

		while (1)
		{
			candidate = _multiplexer.fetch();
			if (candidate.first == -1)
				continue ;
			for (size_t i = 0; i < _servers.size(); i++)
			{
				if (candidate.first == _servers[i].getFD())
				{
					if (candidate.second == aRead)
						;//accept the new connection
					break ;
				}
			}
			for (size_t i = 0; i < _servers.size(); i++)
			{
				for (size_t j = 0; j < _servers[i]._clients.size(); j++)
				{
					if (candidate.first == _servers[j]._clients[j].getFD())
					{
						if (candidate.second == aRead)
							;//_servers[i].handleResponse();
						else if (candidate.second == aWrite)
							;//_servers[i].handleRequest();
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
		std::stringstream	lineStream;

		_configFile.open(_configFileName.c_str());
		if (!_configFile.is_open())
			throw std::runtime_error("Could not open config file");

		while (_configFile.good())
		{
			std::getline(_configFile, line);
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
					_servers.push_back(Server(_configFile));
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