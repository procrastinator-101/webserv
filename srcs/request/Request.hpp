#ifndef REQUEST_HPP
# define REQUEST_HPP

#include <cstddef>
#include <cstring>
#include <string>
#include <vector>

#include <sstream>
#include <fstream>
#include <iomanip>

#include "../stdlib/stdlib.hpp"
#include "../header_field/HeaderField.hpp"

#define NGINY_VAR_PATH_INC	""

namespace ft
{
	class Request
	{
		friend class Client;
		
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			bool						_keepAlive;
			size_t						_bodySize;
			size_t						_contentLength;
			
			std::string					_msg;
			std::string					_method;
			std::string					_path;
			std::string					_version;
			std::vector<HeaderField>	_headers;
			std::string					_bodyFileName;
			std::fstream				_body;
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
			bool	parse(char *buffer, size_t size);

			void	_parseMessage();
			void	_parseStartLine(std::vector<std::string>& msgLines);
			void	_parseHeaders(std::vector<std::string>& msgLines, size_t offset);
		//================================================================================================
		//	Request operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			bool	_checkEndParse();
			void	_deepCopy(const Request& src); // = delete
		//================================================================================================
		//	private methods End
		//================================================================================================

		//================================================================================================
		//	overload << for Sockt
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Request& request);
		//================================================================================================
		//	overload << for Sockt End
		//================================================================================================
	};
}

#endif