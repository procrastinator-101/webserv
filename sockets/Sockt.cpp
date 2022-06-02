#include "Sockt.hpp"
#include <exception>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>

namespace ft
{
	const int		Sockt::defaultBacklog = 10;
	const in_port_t	Sockt::defaultPort = htons(8080);
	const socklen_t	Sockt::addressLen = sizeof(sockaddr_in);
	const in_addr_t	Sockt::defaultAddress = htonl(INADDR_ANY);

	Sockt::Sockt()
	{
		_initialise(defaultAddress, defaultPort, defaultBacklog);
	}

	Sockt::~Sockt()
	{
		if (fd >= 0)
			close(fd);
	}

	Sockt::Sockt(const Sockt& src) : fd(src.fd), backlog(src.backlog), address(src.address)
	{
	}

	Sockt::Sockt(const int& fd, const int& backlog, const sockaddr_in& address) : fd(fd), backlog(backlog), address(address)
	{
	}

	Sockt::Sockt(const in_addr_t& ipAddress, const in_port_t& port, const int& bcklog, const bool& isDry)
	{
		if (isDry)
			_initialise(ipAddress, port, bcklog);
		else
			makeServerSockt(ipAddress, port, bcklog);
	}

	Sockt	&Sockt::operator=(const Sockt& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Sockt::makeServerSockt(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog)
	{
		_initialise(ipAddress, port, bcklog);
		create();
		try
		{
			bind();
			listen();
		}
		catch (std::exception& e)
		{
			close(fd);
			fd = -1;
			throw ;
		}
	}

	void	Sockt::makeClientSockt(const in_addr_t& ipAddress, const in_port_t& port)
	{
		create();
		try
		{
			connect(ipAddress, port);
		}
		catch (std::exception& e)
		{
			close(fd);
			fd = -1;
			throw ;
		}
	}

	void	Sockt::create()
	{
		int	ret;

		ret = socket(AF_INET, SOCK_STREAM, 0);
		if (ret < 0)
			throw std::runtime_error("socket creation failure");
		fd = ret;
	}

	void	Sockt::bind()
	{
		int	ret;

		ret = ::bind(fd, reinterpret_cast<sockaddr *>(&address), address.sin_len);
		if (ret < 0)
			throw std::runtime_error("socket binding failure");
	}

	void	Sockt::listen()
	{
		int	ret;

		ret = ::listen(fd, backlog);
		if (ret < 0)
			throw std::runtime_error("socket listening failure");
	}

	void	Sockt::connect(const in_addr_t& ipAddress, const in_port_t& port)
	{
		int	ret;
		sockaddr_in	dst;

		//set the address that the socket will connect to
		bzero(&dst, addressLen);
		dst.sin_port = port;
		dst.sin_family = AF_INET;
		dst.sin_len = addressLen;
		dst.sin_addr.s_addr = ipAddress;

		ret = ::connect(fd, reinterpret_cast<sockaddr *>(&dst), addressLen);
		if (ret < 0)
			throw std::runtime_error("socket connecting failure");
	}

	Sockt	Sockt::accept()//
	{
		Sockt		ret;
		socklen_t	len;

		ret.fd = ::accept(fd, reinterpret_cast<sockaddr *>(&ret.address), &len);
		if (ret.fd < 0)
			throw std::runtime_error("socket accepting failure");
		return ret;
	}

	void	Sockt::_deepCopy(const Sockt& src)
	{
		fd = src.fd;
		backlog = src.backlog;
		address = src.address;
	}

	void	Sockt::_initialise(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog)
	{
		//set the fd to -1
		fd = -1;

		//set the address that the socket will bind to
		bzero(&address, addressLen);
		address.sin_port = port;
		address.sin_family = AF_INET;
		address.sin_len = addressLen;
		address.sin_addr.s_addr = ipAddress;

		//set the backlog
		backlog = bcklog;
	}

	std::ostream	&operator<<(std::ostream& ostr, const ft::Sockt& sockt)
	{
		const int	fieldSize = 30;

		ostr << std::left;
		ostr << "____________________________ Sockt ____________________________" << std::endl;
		ostr << std::setw(fieldSize) << "fd : " << sockt.fd << std::endl;
		ostr << std::setw(fieldSize) << "backlog : " << sockt.backlog << std::endl;
		ostr << std::setw(fieldSize) << "sin_len : " << sockt.address.sin_len << std::endl;
		ostr << std::setw(fieldSize) << "sin_family : " << sockt.address.sin_family << std::endl;
		ostr << std::setw(fieldSize) << "sin_port : " << sockt.address.sin_port << std::endl;
		ostr << std::setw(fieldSize) << "sin_addr : " << sockt.address.sin_addr.s_addr << std::endl;
		ostr << std::setw(fieldSize) << "sin_zero : " << sockt.address.sin_zero << std::endl;
		ostr << "_______________________________________________________________" << std::endl;
		return ostr;
	}
}