#ifndef CGI_HPP
#define CGI_HPP

#include <set>
#include <sys/time.h>
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
			bool	isTimedOut() const;
			void	selectScript();
			int	execute(Response& response, Request& request);
			void	constructEnv(Response& response, Request& request);
		//================================================================================================
		//	Cgi operations End
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