#include "ServerSockt.hpp"

namespace ft
{
	const int		ServerSockt::defaultBacklog = 128;
	const in_port_t	ServerSockt::defaultPort = htons(8080);
	const in_addr_t	ServerSockt::defaultAddress = htonl(INADDR_ANY);

	ServerSockt::ServerSockt()
	{
	}

	ServerSockt::~ServerSockt()
	{
	}

	ServerSockt::ServerSockt(const std::string& ipAddress, const std::string& port, const int& bcklog) : Sockt(ipAddress, port, bcklog)
	{
	}

	ServerSockt::ServerSockt(const ServerSockt& src) : Sockt(src)
	{
	}

	ServerSockt	&ServerSockt::operator=(const ServerSockt& rop)
	{
		if (this == &rop)
			return *this;
		Sockt::operator=(rop);
		return *this;
	}

	void	ServerSockt::wakeUp()
	{
		open();
		try
		{
			bind();
			listen();
		}
		catch (std::exception& e)
		{
			close();
			throw ;
		}
	}
}