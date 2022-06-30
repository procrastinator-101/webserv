#ifndef CGI_HPP
#define CGI_HPP


#include <signal.h>
#include <sys/time.h>

#include <set>
#include <vector>
#include <string>
#include <cstddef>
#include <stdexcept>

# define CGI_EXEC_TIMEOUT	3000	//milliseconds

namespace ft
{
	class Host;
	class Server;
	class Client;
	class Request;
	class Response;

	class Cgi
	{
		//================================================================================================
		//	Cgi status defintion
		//================================================================================================
		public:
			enum Status
			{
				cSucces,
				cWait,
				cTimeout,
				cError,
			};
		//================================================================================================
		//	Cgi status defintion End
		//================================================================================================
		
		//================================================================================================
		//	static attributes
		//================================================================================================
		private:
			static std::set<std::string>	_envList;
			static std::set<std::string>	_httpForbiddenEnvHeaders;
		//================================================================================================
		//	static attributes End
		//================================================================================================

		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			const char					**_sysEnv;
			const Host					*_host;
			const Server				*_server;
			const Client				*_client;
			std::vector<std::string>	_env;
			int							_pid;
			bool						_isRunning;
			timeval						_begin;
		//================================================================================================
		//	attributes End
		//================================================================================================

		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		private:
			Cgi	&operator=(const Cgi& rhs);//delete

		public:
			Cgi();
			~Cgi();
			
			Cgi(const char **sysEnv, const Host *host, const Server *server, const Client *client);
			Cgi(const Cgi& src);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================
		
		//================================================================================================
		//	Cgi operations
		//================================================================================================
		public:
			void	reset();
			bool	isTimedOut() const;
			void	selectScript();
			int		execute(Response& response, Request& request);
			void	constructEnv(Request& request);

			//returns 0 : normal termination
			//return -1 : an error occured
			//returns 1 : timeout
			//returns 2 : still not done + with not timeout
			Status		timeOut();
		//================================================================================================
		//	Cgi operations End
		//================================================================================================
		
		
		//================================================================================================
		//	Cgi Setters
		//================================================================================================
		public:
			void	setHost(const Host *host);
			void	setSysEnv(const char **sysEnv);
			void	setServer(const Server *server);
			void	setClient(const Client *client);
			void	setPathInfo(const std::string& pathInfo);
			void	setScriptName(const std::string& scriptName);
			void	setPathTranslated(const std::string& pathTranslated);
		//================================================================================================
		//	Cgi Setters End
		//================================================================================================
		


		//================================================================================================
		//	Private methods
		//================================================================================================
		private:
			void	_initialiseHttpForbiddenEnvHeaders();
			void	_setHttpEnvHeaders(const Request& request);
			bool	_isForbiddenHttpHeaderEnv(const std::string& header);

			bool	_isCgiEnv(const std::string& str);

			void	_initialiseEnv();
			void	_initialiseEnvList();

			void	_deepCopy(const Cgi& src);
		//================================================================================================
		//	Private methods End
		//================================================================================================


	};
}

#endif