#include "Sockt.hpp"

namespace ft
{
	const int		Sockt::defaultBacklog = 10;
	const in_port_t	Sockt::defaultPort = htons(8080);
	const socklen_t	Sockt::addressLen = sizeof(sockaddr_in);
	const in_addr_t	Sockt::defaultAddress = htonl(INADDR_ANY);

	Sockt::Sockt(bool isDry) : fd(-1)
	{
		_makeSockt(defaultAddress, defaultPort, defaultBacklog);
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

	Sockt::Sockt(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog) : fd(-1)
	{
		_makeSockt(ipAddress, port, bcklog);
	}

	Sockt	&Sockt::operator=(const Sockt& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Sockt::_makeSockt(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog)
	{
		int	ret;

		//create a socket
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
			throw std::runtime_error("socket creation failure");

		//set the address that the socket will bind to
		bzero(&address, addressLen);
		address.sin_port = port;
		address.sin_family = AF_INET;
		address.sin_len = addressLen;
		address.sin_addr.s_addr = ipAddress;

		//bind to the address
		ret = ::bind(fd, reinterpret_cast<sockaddr *>(&address), address.sin_len);
		if (ret < 0)
		{
			close(fd);
			throw std::runtime_error("socket binding failure");
		}

		//set the backlog
		backlog = bcklog;

		//listen on the socket
		ret = ::listen(fd, backlog);
		if (ret < 0)
		{
			close(fd);
			throw std::runtime_error("socket listening failure");
		}
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

	void	Sockt::accept()
	{

	}

	void	Sockt::connect()
	{

	}

	void	Sockt::create(const in_addr_t& ipAddress, const in_port_t& port, const int bcklog)
	{
		int	ret;

		//create a socket
		fd = socket(AF_INET, SOCK_STREAM, 0);
		if (fd < 0)
			throw std::runtime_error("socket creation failure");

		//set the address that the socket will bind to
		bzero(&address, addressLen);
		address.sin_port = port;
		address.sin_family = AF_INET;
		address.sin_len = addressLen;
		address.sin_addr.s_addr = ipAddress;

		//bind to the address
		ret = ::bind(fd, reinterpret_cast<sockaddr *>(&address), address.sin_len);
		if (ret < 0)
		{
			close(fd);
			throw std::runtime_error("socket binding failure");
		}

		//set the backlog
		backlog = bcklog;

		//listen on the socket
		ret = ::listen(fd, backlog);
		if (ret < 0)
		{
			close(fd);
			throw std::runtime_error("socket listening failure");
		}
	}
	
	void	Sockt::_deepCopy(const Sockt& src)
	{
		fd = src.fd;
		backlog = src.backlog;
		address = src.address;
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