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

#include "../cgi/Cgi.hpp"

#include "../http_status/HttpStatus.hpp"

#include "../mime_type/mimeType.hpp"

#include <dirent.h>
#include <stdlib.h>

# define UPLOAD_BUFFER_SIZE	1048576

namespace ft
{
	class Host;
	class Server;
	class Request;

	class Response
	{
		friend class Client;
		friend class Cgi;
		
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			bool								_isCgiResponse;
			bool								_isGood;
			const Host							*_host;
			size_t								_sent;
			std::string							_msg;

			Cgi									_cgi;

			bool								_keepAlive;
			size_t								_contentLength;

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

			Response();

		private:
			Response(const Response& src);
			Response	&operator=(const Response& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Response operations
		//================================================================================================
		public:
			bool	timeOut();
			void	reset();
			bool	isFinished();
			
			std::pair<bool, Transmission>	send(int fd);

			void	build(const std::vector<Host *>& hosts, Request& request);
		//================================================================================================
		//	Response operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_constructBody();
			void	_constructHeaders();
			void	_constructStatusLine();
			void	_constructHead(Request& request);

			std::string	getStatusLine() const;

			void	_constructErrorResponse(const HttpStatus& status);

			void	_parseCgiResponse();

			void	_formatMessage();

			void	_prepareCgiResponseHead();


			void	_initiateCgi(Request& request, const std::string& scriptPath, const std::string& filePath);

			void	_prepare(const Host* host, Request& request);
			void	_handleGetMethod(Request& request, const std::pair<std::string, Location *>& location);
			void	_handlePostMethod(Request& request, const std::pair<std::string, Location *>& location, size_t MaxBodySize);
			void	_handleDeleteMethod(Request& request, const std::pair<std::string, Location *>& location);

			std::pair<std::string, Location *>	get_matched_location_for_request_uri(const std::string path, const std::map<std::string, Location *> locations);
			std::string		prepare_path(const std::string& location_root, const std::string &uri);
			bool			is_method_allowded_in_location(const std::string &method, const Location *location);
			void			_handleDirInGet(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			void			_handleFileInGet(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			std::string		IsDirHasIndexFiles(const std::pair<std::string, Location *>& location, std::string& path);
			void			_handleDirIn_POST(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			void			_handleFileIn_POST(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			void			_handleDirIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			void			_handleFileIn_DELETE(const std::pair<std::string, Location *>& location, std::string& path, Request& request);
			int				DeleteFolderContent(std::string& path);
			void			getFileFromStatus(const Host *host, int code);
			void			_uploadfile(Request& request, const std::string& path);
			void			_prepare_indixing(std::string& path, Request& request);

			bool		matched_ext(std::map<std::string, std::string> cgis, std::string& path, std::string& cgi_ext);

			void		setContentType(std::string& path);

			const Host	*_fetchTargetedHost(const std::vector<Host *>& hosts, const std::string& name);

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