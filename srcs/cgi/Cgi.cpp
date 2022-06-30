#include "Cgi.hpp"

#include "../request/Request.hpp"
#include "../response/Response.hpp"

#include "../client/Client.hpp"
#include "../server/Server.hpp"
#include <new>
#include <sys/_types/_size_t.h>
#include <unistd.h>


namespace ft
{
	std::set<std::string>	Cgi::_envList;
	std::set<std::string>	Cgi::_httpForbiddenEnvHeaders;

	Cgi::Cgi() :	_sysEnv(0), _host(0), _server(0), _client(0), _env(), _pid(-1), _isRunning(false), _begin()
	{
	}

	Cgi::~Cgi()
	{
	}

	
	Cgi::Cgi(const char **sysEnv, const Host *host, const Server *server, const Client *client) :	_sysEnv(sysEnv), _host(host),
																									_server(server), _client(client),
																									_env(), _pid(-1), _isRunning(false),
																									_begin()
	{
		if (!sysEnv)
			throw std::invalid_argument("Cgi:: sysEnv is null");
		_initialiseEnv();
	}

	Cgi::Cgi(const Cgi& src) :	_sysEnv(src._sysEnv), _host(src._host), _server(src._server), _client(src._client), _env(src._env), 
								_pid(src._pid), _isRunning(src._isRunning), _begin(src._begin)
	{
	}


	Cgi	&Cgi::operator=(const Cgi& rhs)
	{
		if (this == &rhs)
			return *this;
		_deepCopy(rhs);
		return *this;
	}

	bool	Cgi::isTimedOut() const
	{
		if (!_isRunning)
			return false;
		if (getTimeStamp(_begin) >= CGI_EXEC_TIMEOUT)
			return true;
		return false;
	}

	Cgi::Status	Cgi::timeOut()
	{
		int	ret;
		int	status;

		ret = waitpid(_pid, &status, WNOHANG);

		if (!ret)
		{
			if (isTimedOut())
			{
				kill(_pid, SIGKILL);
				return cTimeout;
			}
			else
				return cWait;
		}
		else if (ret < 0)
		{
			kill(_pid, SIGKILL);
			return cError;
		}
		else if (!WIFEXITED(status) || WEXITSTATUS(status))
			return cError;
		return cSucces;
	}

	Cgi::Status	Cgi::execute(Response& response, Request& request)
	{
		int	ret;
		int	fd[2];
		

		constructEnv(request);
		_pid = fork();
		if (_pid < 0)
			return  cError;

		gettimeofday(&_begin, 0);
		if (!_pid)
		{
			fd[0] = open(request._bodyFileName.c_str(), O_RDONLY);
			if (fd[0] < 0)
				exit(EXIT_FAILURE);
			fd[1] = open(response._bodyFileName.c_str(), O_WRONLY | O_TRUNC);
			if (fd[1] < 0)
			{
				close(fd[0]);
				exit(EXIT_FAILURE);
			}
			ret = dup2(STDIN_FILENO, fd[0]);
			if (ret < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(EXIT_FAILURE);
			}
			ret = dup2(STDOUT_FILENO, fd[1]);
			if (ret < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(EXIT_FAILURE);
			}
			// ret = execve(, , );
			close(fd[0]);
			close(fd[1]);
			exit(EXIT_FAILURE);
		}
		return timeOut();
	}

	int	Cgi::_runScript(int fd[2])
	{
		char **env;
		char **args;

		env = new(std::nothrow) char *[_env.size() + 1];
		if (!env)
		{
			close(fd[0]);
			close(fd[1]);
			exit(EXIT_FAILURE);
		}
		args = new(std::nothrow) char *[2];
		for (size_t i = 0; i < _env.size(); i++)
			env[i] = _env[i].c_str();
		env[_env.size()] = 0;
		args[0] = _scriptName.c_str();
		args[1] = 0;
		execve(_scriptName.c_str(), args, env);
	}

	void	Cgi::constructEnv(Request& request)
	{
		std::map<std::string, std::string>::iterator	it;

		//clean cgi envs from env
		for (size_t i = 0;  i < _env.size(); i++)
		{
			if (_isCgiEnv(_env[i]))
				_env.erase(_env.begin() + i);
		}

		//AUTH_TYPE
		it  = request._headers.find("auth-scheme");
		if (it != request._headers.end())
			_env.push_back("AUTH_TYPE=" + it->second);
		
		//CONTENT_LENGTH
		if (request._bodySize)
			_env.push_back("CONTENT_LENGTH=" + ztoa(request._bodySize));
		
		//CONTENT_TYPE
		it  = request._headers.find("Content-Type");
		if (it != request._headers.end())
			_env.push_back("CONTENT_TYPE=" + it->second);
		
		//GATEWAY_INTERFACE
		_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

		//PATH_INFO

		//PATH_TRANSLATED

		//QUERY_STRING
		_env.push_back("QUERY_STRING=" + request._querry);

		//REMOTE_ADDR
		_env.push_back("REMOTE_ADDR=" + _client->getIpAddress());

		//REMOTE_HOST
		_env.push_back("REMOTE_HOST=" + _client->getIpAddress());

		//REMOTE_USER

		//REQUEST_METHOD
		_env.push_back("REQUEST_METHOD=" + request._method);
		
		//SCRIPT_NAME

		//SERVER_NAME
		_env.push_back("SERVER_NAME=" + request._headers["Host"]);

		//SERVER_PORT
		_env.push_back("SERVER_PORT=" + _server->getPortNumber());

		//SERVER_PROTOCOL
		_env.push_back("SERVER_PROTOCOL=HTTP/1.1");

		//SERVER_SOFTWARE
		_env.push_back("SERVER_SOFTWARE=nginy/1");

		_setHttpEnvHeaders(request);
	}

	void	Cgi::_setHttpEnvHeaders(const Request& request)
	{
		for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it)
		{
			if (!_isForbiddenHttpHeaderEnv(it->first))
				_env.push_back(http::headerToEnv(it->first) + "=" + it->second);
		}
	}

	bool	Cgi::_isForbiddenHttpHeaderEnv(const std::string& header)
	{
		if (_httpForbiddenEnvHeaders.empty())
			_initialiseHttpForbiddenEnvHeaders();
		return _httpForbiddenEnvHeaders.find(header) != _httpForbiddenEnvHeaders.end();
	}

	void	Cgi::_initialiseHttpForbiddenEnvHeaders()
	{
		_httpForbiddenEnvHeaders.insert("Connection");
		_httpForbiddenEnvHeaders.insert("Content-Type");
		_httpForbiddenEnvHeaders.insert("Content-Length");
		_httpForbiddenEnvHeaders.insert("Authorization");
	}

	void	Cgi::_initialiseEnv()
	{
		for (size_t i = 0; _sysEnv[i]; i++)
			_env.push_back(_sysEnv[i]);
	}

	bool	Cgi::_isCgiEnv(const std::string& str)
	{
		bool		ret;
		size_t		pos;
		std::string	key;

		if (_envList.empty())
			_initialiseEnvList();
		pos = str.find('=');
		if (pos == std::string::npos)
			key = str;
		else
			key.assign(str, pos);
		ret = _envList.find(key) != _envList.end();
		if (ret)
			return true;
		return key.compare(0, 5, "HTTP_") == 0;
	}

	void	Cgi::_initialiseEnvList()
	{
		_envList.insert("AUTH_TYPE");
		_envList.insert("CONTENT_LENGTH");
		_envList.insert("CONTENT_TYPE");
		_envList.insert("GATEWAY_INTERFACE");
		_envList.insert("PATH_INFO");
		_envList.insert("PATH_TRANSLATED");
		_envList.insert("QUERY_STRING");
		_envList.insert("REMOTE_ADDR");
		_envList.insert("REMOTE_HOST");
		_envList.insert("REMOTE_IDENT");
		_envList.insert("REMOTE_USER");
		_envList.insert("REQUEST_METHOD");
		_envList.insert("SCRIPT_NAME");
		_envList.insert("SERVER_NAME");
		_envList.insert("SERVER_PORT");
		_envList.insert("SERVER_PROTOCOL");
		_envList.insert("SERVER_SOFTWARE");
	}

	void	Cgi::reset()
	{
		_sysEnv = 0;
		_host = 0;
		_server = 0;
		_client = 0;
		_env.clear();
		_pid = -1;
		_isRunning = false;
	}

	void	Cgi::setPathInfo(const std::string& pathInfo)
	{
		_env.push_back("PATH_INFO=" + pathInfo);
	}

	void	Cgi::setScriptName(const std::string& scriptName)
	{
		_scriptName = scriptName;
		_env.push_back("SCRIPT_NAME=" + scriptName);
	}

	void	Cgi::setPathTranslated(const std::string& pathTranslated)
	{
		_env.push_back("PATH_TRANSLATED=" + pathTranslated);
	}

	void	Cgi::setHost(const Host *host)
	{
		_host = host;
	}

	void	Cgi::setSysEnv(const char **sysEnv)
	{
		_sysEnv = sysEnv;
	}
	
	void	Cgi::setServer(const Server *server)
	{
		_server = server;
	}
	
	void	Cgi::setClient(const Client *client)
	{
		_client = client;
	}

	void	Cgi::_deepCopy(const Cgi& src)
	{
		(void)src;
	}
}