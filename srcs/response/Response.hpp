#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <string>
#include <sys/_types/_size_t.h>
#include <vector>
#include <iomanip>
#include <fstream>


#include "../sockets/Sockt.hpp"
#include "../stdlib/stdlib.hpp"

#include "../request/Request.hpp"

#include "../host/Host.hpp"

#include "../http_status/HttpStatus.hpp"

namespace ft
{
	class Host;
	class Request;

	class Response
	{
		friend class Client;
		
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			size_t								_sent;
			size_t								_contentLength;
			std::string							_msg;

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
			Response();
			~Response();

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
			void	_handleGetMethod(const Host* host, const Request& request);
			void	_handlePostMethod(const Host* host, const Request& request);
			void	_handleDeleteMethod(const Host* host, const Request& request);


			void	_constructStatusLine();
			void	_constructHeaders(const Request& request);
			void	_constructBody(const Request& request);

			void	_buildBadRequestResponse();
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
}

#endif