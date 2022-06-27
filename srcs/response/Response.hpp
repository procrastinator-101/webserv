#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <vector>

#include <iomanip>
#include <fstream>


#include "../sockets/Sockt.hpp"
#include "../stdlib/stdlib.hpp"

#include "../request/Request.hpp"

#include "../host/Host.hpp"

#include "../http_status/HttpStatus.hpp"

#include <dirent.h>
#include <stdlib.h>

# define UPLOAD_BUFFER_SIZE	1048576
# define CGI_ENV_SIZE		13

namespace ft
{
	class Host;
	class Server;
	class Request;

	class Response
	{
		friend class Client;
		
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			const char							**_env;
			size_t								_sent;
			std::string							_msg;

			bool								_keepAlive;
			size_t								_contentLength;

			std::string							_version;
			HttpStatus							_status;
			std::map<std::string, std::string>	_headers;
			std::string							_bodyFileName;
			std::fstream						_body;
		//================================================================================================
		//	attributes End
		//================================================================================================

		
		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			~Response();

			Response(const char **env);

		private:
			Response();
			Response(const Response& src);
			Response	&operator=(const Response& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Response operations
		//================================================================================================
		public:
			void	reset();
			bool	send(int fd);
			void	build(const std::vector<Host *>& hosts, const Request& request);
		//================================================================================================
		//	Response operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:

			void	_prepare(const Host* host, const Request& request);
			void	_handleGetMethod(const Request& request, const std::pair<std::string, Location *>& location);
			void	_handlePostMethod(const Request& request, const std::pair<std::string, Location *>& location);
			void	_handleDeleteMethod(const Host* host, const Request& request, const std::pair<std::string, Location *>& location);

			std::pair<std::string, Location *>	get_matched_location_for_request_uri(const std::string path, const std::map<std::string, Location *> locations);
			std::string		prepare_path(const std::string& location_root, const std::string &uri);
			bool			is_method_allowded_in_location(const std::string &method, const Location *location);
			void			_handleDirInGet(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleFileInGet(const std::pair<std::string, Location *>& location, std::string& path);
			std::string		IsDirHasIndexFiles(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleDirIn_POST(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleFileIn_POST(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleDirIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleFileIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path);
			int				DeleteFolderContent(std::string& path);
			void			getFileFromStatus(const Host *host, int code);
			void			_uploadfile(const Request& request, const std::string& path);

			void	_constructStatusLine();
			void	_constructHeaders(const Request& request);
			void	_constructBody(const Request& request);

			void	_buildBadRequestResponse();
			const Host	*_fetchTargetedHost(const std::vector<Host *>& hosts, const std::string& name);


			void	_cgi(const Request& request, const Server& server);
			bool	_isCgiEnv(const char *str);
			void	_constructCgiEnv(const Request& request, const Server& server);
			void	_setCgiEnv(char **cgiEnv, size_t size, const std::string& key, const std::string& value);
			void	_initialiseCgiEnvList(std::set<std::string>& cgiEnvList);


			void	_deepCopy(const Response& src);
		//================================================================================================
		//	private methods End
		//================================================================================================


		//================================================================================================
		//	overload << for Response
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Response& response);
		//================================================================================================
		//	overload << for Response End
		//================================================================================================
	};

	//utils
		bool	sort_pair_string(const std::pair<std::string, Location *>& lhs, const std::pair<std::string, Location *>& rhs);
}

#endif