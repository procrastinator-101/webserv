#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <cstddef>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iomanip>



#include "../sockets/Sockt.hpp"
#include "../stdlib/stdlib.hpp"

# define BUFFER_SIZE 1024

namespace ft
{
	class Request
	{
		friend class Client;
		friend class Response;

		enum Status
		{
			good,
			bad,
			fatal
		};
		
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			bool								_isTrailerReached;
			size_t								_chunkLen;
			size_t								_chunkSize;
			Status								_status;
			size_t								_bodySize;
			std::string							_msg;

			bool								_isChunked;
			bool								_keepAlive;
			size_t								_contentLength;
			std::set<std::string>				_trailerHeaders;

			std::string							_method;
			std::string							_path;
			std::string							_version;
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
			Request();
			~Request();

			Request(std::string& msg);
		
		private:
			Request(const Request& src); // = delete
			Request	&operator=(const Request& rop); // = delete
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Request operations
		//================================================================================================
		public:
			void	reset();
			bool	isValid();
			bool	receive(int fd);
		//================================================================================================
		//	Request operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:

			bool	_endParse();
			bool	_parse(char *buffer, size_t size);
			bool	_fillBody(char *buffer, size_t size);
			bool	_fillChunkedBody(char* &buffer, size_t size);

			Status	_parseMessage();//returns true if message is fataly bad
			Status	_parseStartLine(std::vector<std::string>& msgLines);
			Status	_parseHeaders(std::vector<std::string>& msgLines, size_t offset);

			Status	_setHeader(const std::string& key, const std::string& value);

			Status	_checkStartLine() const;
			Status	_checkHeaders() const;
			Status	_checkBody() const;

			bool	_isTrailerAllowedHeader(std::string& header) const;
			void	_fillTrailerDisallowedHeaders(std::set<std::string>& disallowedHeaders) const;

			void	_deepCopy(const Request& src); // = delete
		//================================================================================================
		//	private methods End
		//================================================================================================

		//================================================================================================
		//	Request exceptions
		//================================================================================================
		public:
			class badRequest : public std::exception
			{
				private:
					std::string	_str;
				
				public:
					badRequest();
					~badRequest() throw();
					badRequest(const std::string& str);

					const char	*what() const throw();
			};
		//================================================================================================
		//	Request exceptions End
		//================================================================================================


		//================================================================================================
		//	overload << for Request
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Request& request);
		//================================================================================================
		//	overload << for Request End
		//================================================================================================
	};
}

#endif