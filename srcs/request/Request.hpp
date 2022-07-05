#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <cstddef>
#include <cstring>
#include <exception>
#include <string>
#include <sys/_types/_timeval.h>
#include <utility>
#include <vector>

#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <iomanip>


#include "../sockets/Sockt.hpp"
#include "../stdlib/stdlib.hpp"

# define BUFFER_SIZE 1024


# define HTTP_NEWLINE	"\r\n"
# define HTTP_BLANKlINE	"\r\n\r\n"

# define HTTP_WHITE_SPACES	"\t\v\f "


# define MAX_REQUEST_LINE_LENGTH	10000
# define RECV_TIMEOUT				30000

namespace ft
{
	class Request
	{
		friend class Client;
		friend class Response;
		friend class Cgi;

		public:
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
			bool								_isReceiving;
			timeval								_begin;

			bool								_isInChunk;
			size_t								_chunkLen;
			size_t								_chunkSize;

			bool								_isTrailerSet;
			bool								_isTrailerReached;

			bool								_notImplemented;
			Status								_status;
			bool								_isInBody;
			size_t								_bodySize;
			std::string							_buffer;


			bool								_isChunked;
			bool								_keepAlive;
			size_t								_contentLength;
			std::string							_querry;
			std::set<std::string>				_trailerHeaders;
			std::vector<std::string>			_cookies;


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

			Status	status() const;
			bool	isReceiving() const;

			bool	timeOut() const;

			std::pair<bool, Transmission>	receive(int fd);
		//================================================================================================
		//	Request operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_updateRecvState();

			bool	_endBody();
			bool	_parse(char *buffer, size_t size);

			std::string	_decodePercentUri(std::string& uri);


			//return true if status is fatal, false otehrwise
			bool	_setStatus(Status status);

			Request::Status	_parseMethod(std::string& method);
			Request::Status	_parseUri(std::string& uri);
			Request::Status	_parseVersion(std::string& version);

			//return true if receiving has ended
			bool	_parseHead();
			bool	_fetchTrailerPart();
			bool	_fetchChunkedBody();
			bool	_fetchBody(char *buffer, size_t size);

			void	_resetChunk();
			bool	_beginChunk();

			Status	_parseRequestLine(std::vector<std::string>& msgLines);
			Status	_parseHeaders(std::vector<std::string>& msgLines, size_t offset);

			Status	_setHeader(const std::string& key, const std::string& value);
			Status	_setTrailerHeaders(std::string& line);

			Status	_checkBody() const;
			Status	_checkHeaders() const;
			Status	_checkStartLine() const;

			bool	_isTrailerAllowedHeader(std::string& header) const;
			void	_fillTrailerDisallowedHeaders(std::set<std::string>& disallowedHeaders) const;

			void	_deepCopy(const Request& src); // = delete
		//================================================================================================
		//	private methods End
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