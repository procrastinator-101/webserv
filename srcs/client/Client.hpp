#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "../sockets/Sockt.hpp"
#include "../request/Request.hpp"
#include "../response/Response.hpp"
#include <cstddef>

#define HTTP_NEWLINE "\r\n"

namespace ft
{
	class Server;

	class Client
	{
		friend class Nginy;

		//================================================================================================
		//	attributes
		//================================================================================================
		private:
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

		private:
			Client(const Client& src);
			Client	&operator=(const Client& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Client operations
		//================================================================================================
		public:
			bool	keepAlive() const;
			/**
			 * @brief 
			 * 
			 * @return true : the client is ready(request handled completely) to send the response
			 * @return false : the client is not ready to send the response
			 */
			bool	handleRequest(const Server& server);

			/**
			 * @brief : takes the server that the client is interracting with
			 * 
			 * @return true : the client is ready(reponse completely sent) to receive new requests
			 * @return false : the client is not ready to receive requests
			 */
			bool	handleResponse();
		//================================================================================================
		//	Client operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Client& src);

			void	_prepareResponse();
		//================================================================================================
		//	private methods End
		//================================================================================================

		//================================================================================================
		//	overload << for Client
		//================================================================================================
		public:
			friend std::ostream	&operator<<(std::ostream& ostr, const Client& client);
		//================================================================================================
		//	overload << for Client End
		//================================================================================================
	};
}

#endif