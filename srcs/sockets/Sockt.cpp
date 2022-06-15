#include "Sockt.hpp"

namespace ft
{
	const socklen_t	Sockt::addressLen = sizeof(sockaddr_in);

	Sockt::Sockt() : fd(-1)
	{
	}

	Sockt::~Sockt()
	{
	}

	Sockt::Sockt(const std::string& ipAddress, const std::string& port, const int& bcklog)
	{
		int			hPort;
		in_port_t	inPort;
		in_addr_t	inAddr;

		inAddr = inet_addr(ipAddress.c_str());
		if (inAddr == static_cast<in_addr_t>(-1))
			throw std::invalid_argument("Sockt:: invalid ip adress");
		if (port.empty() || !isnumber(port))
			throw std::invalid_argument("Sockt:: invalid port");
		hPort = ::atoi(port.c_str());
		if (hPort < 0 || hPort > 65535)
			throw std::invalid_argument("Sockt:: invalid port number");
		inPort = htons(hPort);
		if (backlog < 0)
			throw std::invalid_argument("Sockt:: invalid backlog");
		_initialise(inAddr, inPort, bcklog);
	}

	Sockt::Sockt(const Sockt& src) : fd(src.fd), backlog(src.backlog), address(src.address)
	{
	}

	Sockt	&Sockt::operator=(const Sockt& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Sockt::open()
	{
		int	ret;

		ret = socket(AF_INET, SOCK_STREAM, 0);
		if (ret < 0)
			throw std::runtime_error("socket:: open failed");
		fd = ret;
	}

	void	Sockt::bind()
	{
		int	ret;

		ret = ::bind(fd, reinterpret_cast<sockaddr *>(&address), addressLen);//!!!
		if (ret < 0)
			throw std::runtime_error("socket:: bind failed");
	}

	void	Sockt::listen()
	{
		int	ret;

		ret = ::listen(fd, backlog);
		if (ret < 0)
			throw std::runtime_error("socket:: listen failed");
	}

	void	Sockt::connect(const in_addr_t& ipAddress, const in_port_t& port)
	{
		int	ret;
		sockaddr_in	dst;

		//set the address that the socket will connect to
		bzero(&dst, addressLen);
		dst.sin_port = port;
		dst.sin_family = AF_INET;
		// dst.sin_len = addressLen;!!
		dst.sin_addr.s_addr = ipAddress;

		ret = ::connect(fd, reinterpret_cast<sockaddr *>(&dst), addressLen);
		if (ret < 0)
			throw std::runtime_error("socket:: connect failed");
	}

	void	Sockt::connect(const Sockt dst)
	{
		int	ret;
		
		ret = ::connect(fd, reinterpret_cast<const sockaddr *>(&dst.address), addressLen);
		if (ret < 0)
			throw std::runtime_error("socket:: connect failed");
	}

	Sockt	Sockt::accept()
	{
		Sockt		ret;
		socklen_t	len;

		ret.fd = ::accept(fd, reinterpret_cast<sockaddr *>(&ret.address), &len);
		if (ret.fd < 0)
			throw std::runtime_error("socket:: accept failed");
		return ret;
	}

	void	Sockt::close()
	{
		if (fd >= 0)
			::close(fd);
		fd = -1;
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
		// address.sin_len = addressLen;
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
		// ostr << std::setw(fieldSize) << "sin_len : " << sockt.address.sin_len << std::endl;
		ostr << std::setw(fieldSize) << "sin_family : " << sockt.address.sin_family << std::endl;
		ostr << std::setw(fieldSize) << "sin_port : " << sockt.address.sin_port << std::endl;
		ostr << std::setw(fieldSize) << "sin_addr : " << sockt.address.sin_addr.s_addr << std::endl;
		ostr << std::setw(fieldSize) << "sin_zero : " << sockt.address.sin_zero << std::endl;
		ostr << "_______________________________________________________________" << std::endl;
		return ostr;
	}
}