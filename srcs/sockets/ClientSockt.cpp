#include "ClientSockt.hpp"
#include "Sockt.hpp"

namespace ft
{
	ClientSockt::ClientSockt()
	{
	}

	ClientSockt::~ClientSockt()
	{
	}


	ClientSockt::ClientSockt(const ClientSockt& src) : Sockt(src)
	{
	}

	ClientSockt	&ClientSockt::operator=(const ClientSockt& rop)
	{
		if (this == &rop)
			return *this;
		Sockt::operator=(rop);
		return *this;
	}

	void	ClientSockt::wakeUp(const std::string& ipAddress, const std::string& port)
	{
		Sockt	dst(ipAddress, port, 0);

		open();
		try
		{
			connect(dst);
		}
		catch (std::exception& e)
		{
			close();
			throw ;
		}
	}
}