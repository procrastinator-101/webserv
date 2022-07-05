#include "Cgi.hpp"

#include "../request/Request.hpp"
#include "../response/Response.hpp"

#include "../client/Client.hpp"
#include "../server/Server.hpp"
#include <new>
#include <string>
#include <sys/_types/_size_t.h>
#include <unistd.h>


namespace ft
{
	std::set<std::string>	Cgi::_httpForbiddenEnvHeaders;

	Cgi::Cgi() :	_host(0), _server(0), _client(0), _inputFile(), _scriptPath(), _env(), _pid(-1), _isRunning(false), _begin()
	{
	}

	Cgi::~Cgi()
	{
	}

	
	Cgi::Cgi(const Host *host, const Server *server, const Client *client) :	_host(host), _server(server), _client(client),
																				_inputFile(), _scriptPath(), _env(), _pid(-1),
																				_isRunning(false), _begin()
	{
	}

	Cgi::Cgi(const Cgi& src) :	_host(src._host), _server(src._server), _client(src._client), _inputFile(src._inputFile),
								_scriptPath(src._scriptPath), _env(src._env), _pid(src._pid), _isRunning(src._isRunning), 
								_begin(src._begin)
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
		if (getTimeStamp(_begin) >= CGI_EXEC_TIMEOUT)
			return true;
		return false;
	}

	bool	Cgi::isRunning() const
	{
		return _isRunning;
	}

	Cgi::Status	Cgi::timeOut()
	{
		int	ret;
		int	status;

		ret = waitpid(_pid, &status, WNOHANG);

		_isRunning = false;
		if (!ret)
		{
			if (isTimedOut())
			{
				kill(_pid, SIGKILL);
				return cTimeout;
			}
			else
			{
				_isRunning = true;
				return cWait;
			}
		}
		else if (ret < 0)
		{
			kill(_pid, SIGKILL);
			return cError;
		}
		else if (!WIFEXITED(status))
			return cError;
		return cSucces;
	}

	Cgi::Status	Cgi::execute(Response& response, Request& request)
	{
		int	ret;
		int	fd[2];

		constructEnv(request);
		response._bodyFileName = std::string(NGINY_VAR_CGI_PATH) + "/" + getRandomFileName("cgi");
		_pid = fork();
		if (_pid < 0)
			return  cError;

		gettimeofday(&_begin, 0);
		_isRunning = true;
		if (!_pid)
		{
			fd[0] = open(request._bodyFileName.c_str(), O_RDONLY);
			if (fd[0] < 0)
				exit(EXIT_FAILURE);
			fd[1] = open(response._bodyFileName.c_str(), O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
			if (fd[1] < 0)
			{
				close(fd[0]);
				exit(EXIT_FAILURE);
			}
			std::cout << "cgi:: bodyFileName : " << response._bodyFileName << std::endl;
			close(STDIN_FILENO);
			ret = dup2(fd[0], STDIN_FILENO);
			if (ret < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(EXIT_FAILURE);
			}
			std::cout << "cgi envs" << std::endl;
			for (size_t i = 0; i < _env.size(); i++)
				std::cout << _env[i] << std::endl;
			close(STDOUT_FILENO);
			ret = dup2(fd[1], STDOUT_FILENO);
			if (ret < 0)
			{
				close(fd[0]);
				close(fd[1]);
				exit(EXIT_FAILURE);
			}
			_runScript(fd);
			close(fd[0]);
			close(fd[1]);
			exit(EXIT_FAILURE);
		}
		return timeOut();
	}

	void	Cgi::_runScript(int fd[2])
	{
		char **env;
		char **args;

		//set env for cgi script
		env = vec2arr(_env);
		if (!env)
		{
			close(fd[0]);
			close(fd[1]);
			exit(EXIT_FAILURE);
		}

		//set args for the cgi script
		args = _getSciptArgs();
		if (!env)
		{
			close(fd[0]);
			close(fd[1]);
			destroy2arr(env, _env.size() + 1);
			exit(EXIT_FAILURE);
		}
		execve(args[0], args, env);
	}

	char	**Cgi::_getSciptArgs() const
	{
		char	**args;

		args = new (std::nothrow) char *[3];
		if (!args)
			return 0;
		args[0] = ft_strdup(_scriptPath.c_str());
		args[1] = ft_strdup(_inputFile.c_str());
		args[2] = 0;
		if (!args[0] || !args[1])
		{
			destroy2arr(args, 3);
			return 0;
		}
		return args;
	}

	void	Cgi::constructEnv(Request& request)
	{
		std::map<std::string, std::string>::iterator	it;

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
		_env.push_back("SERVER_SOFTWARE=Nginy/1");

		//REDIRECT_STATUS : php specific
		_env.push_back("REDIRECT_STATUS=200");

		_setHttpEnvHeaders(request);
	}

	void	Cgi::_setHttpEnvHeaders(const Request& request)
	{
		for (std::map<std::string, std::string>::const_iterator it = request._headers.begin(); it != request._headers.end(); ++it)
		{
			if (!_isForbiddenHttpHeaderEnv(it->first))
				_env.push_back(http::headerToEnv(it->first) + "=" + it->second);
		}
		//set Cookies
		for (size_t i = 0; i < request._cookies.size(); i++)
			_env.push_back("HTTP_COOKIE=" + request._cookies[i]);
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

	void	Cgi::reset()
	{
		_host = 0;
		_server = 0;
		_client = 0;

		_inputFile.clear();
		_scriptPath.clear();

		_env.clear();
		_pid = -1;
		_isRunning = false;
	}

	void	Cgi::setPathInfo(const std::string& pathInfo)
	{
		_env.push_back("PATH_INFO=" + pathInfo);
	}

	void	Cgi::setInputFile(const std::string& inputFile)
	{
		_inputFile = inputFile;
	}

	void	Cgi::setScriptName(const std::string& scriptName)
	{
		_env.push_back("SCRIPT_NAME=" + scriptName);
	}

	void	Cgi::setPathTranslated(const std::string& pathTranslated)
	{
		_env.push_back("PATH_TRANSLATED=" + pathTranslated);
	}

	void	Cgi::setScriptFileName(const std::string& scriptFileName)
	{
		_env.push_back("SCRIPT_FILENAME=" + scriptFileName);
	}

	void	Cgi::setScriptPath(const std::string& scriptPath)
	{
		_scriptPath = scriptPath;
	}

	void	Cgi::setHost(const Host *host)
	{
		_host = host;
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