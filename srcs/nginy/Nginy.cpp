#include "Nginy.hpp"
#include <sys/_types/_fd_def.h>

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

	void	Nginy::select()
	{
		fd_set	monitoredSockets;

		monitoredSockets = _getMonitoredSockets();
	}
	
	fd_set	Nginy::_getMonitoredSockets() const
	{
		fd_set	ret;

		FD_ZERO(&ret);
		for (std::vector<Server>::size_type i = 0; i < _servers.size(); i++)
		{
			
		}
	}

	void	Nginy::_parseConfigFile()
	{

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