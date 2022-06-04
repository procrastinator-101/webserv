#ifndef HTTPSTATUS_HPP
# define HTTPSTATUS_HPP

#include <map>
#include <string>

namespace ft
{
	/**
	 * @brief 
	 *	HttpStatus class offers a remap of a HTTP status code to its corresponding
	 *	HTTP code message.
	 */
	
	class HttpStatus
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		public:
			int	code;
			std::string	msg;

		private:
			static std::map<int, const std::string>	_library;
		//================================================================================================
		//	attributes End
		//================================================================================================
		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			~HttpStatus();

			HttpStatus(int code = 200);
			HttpStatus(std::string msg);
			HttpStatus(const HttpStatus& src);

			HttpStatus	&operator=(const HttpStatus& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	HttpStatus operations
		//================================================================================================
		public:
			static int	resolve(const std::string msg);
			static std::string	resolve(const int code);
		//================================================================================================
		//	HttpStatus operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			static void	_initialiseLibrary();
			static void	_setInformationalResponseCodes();//100-200
			static void	_setSuccessCodes();//200-300
			static void	_setRedirectionCodes();//300-400
			static void	_setClientErrorCodes();//400-500
			static void	_setServerErrorCodes();//500-600

			void	_deepCopy(const HttpStatus& src);
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif