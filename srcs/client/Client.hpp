#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../sockt/Sockt.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"

namespace ft
{
	enum action
	{
		aWrite,
		aRead,
		aExcept
	};
	
	class Client
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			bool	_keepAlive;
			Sockt	_sockt;
			Request	_request;
			Response	_response;
		//================================================================================================
		//	attributes End
		//================================================================================================

		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Client();
			~Client();

			Client(const Client& src);

			Client	&operator=(const Client& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Client operations
		//================================================================================================
		public:
		//================================================================================================
		//	Client operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Client& src);
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif