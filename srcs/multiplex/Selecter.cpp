#include "Selecter.hpp"
#include "Action.hpp"

namespace ft
{
	Selecter::Selecter()
	{
		FD_ZERO(&_readfds);
		FD_ZERO(&_writefds);
		FD_ZERO(&_exceptfds);
	}

	Selecter::~Selecter()
	{
	}

	Selecter::Selecter(const Selecter& src) :	_readfds(src._readfds), _writefds(src._writefds), _exceptfds(src._exceptfds),
												_readMonitered(src._readMonitered), _writeMonitered(src._writeMonitered),
												_exceptMonitered(src._exceptMonitered)
	{
	}

	Selecter	&Selecter::operator=(const Selecter& rop)
	{
		if (this == &rop)
			return *this;
		_deepCopy(rop);
		return *this;
	}

	void	Selecter::add(int fd, int options)
	{
		if (options & aRead)
		{
			FD_SET(fd, &_readfds);
			_readMonitered.insert(fd);
		}
		if (options & aWrite)
		{
			FD_SET(fd, &_writefds);
			_writeMonitered.insert(fd);
		}
		if (options & aExcept)
		{
			FD_SET(fd, &_exceptfds);
			_exceptMonitered.insert(fd);
		}
	}

	void	Selecter::del(int fd, int options)
	{
		if (options & aRead)
		{
			FD_CLR(fd, &_readfds);
			_readMonitered.erase(fd);
		}
		if (options & aWrite)
		{
			FD_CLR(fd, &_writefds);
			_writeMonitered.erase(fd);
		}
		if (options & aExcept)
		{
			FD_CLR(fd, &_exceptfds);
			_exceptMonitered.erase(fd);
		}
	}

	std::pair<int, Action>	Selecter::fetch(unsigned long milliseconds) const
	{
		int		ret;
		int		nfds;
		timeval	timeout;
		fd_set	_readfdsCopy;
		fd_set	_writefdsCopy;
		fd_set	_exceptfdsCopy;

		//set nfds
		if (!_readMonitered.empty())
			nfds = *_readMonitered.rbegin();
		if (!_writeMonitered.empty())
			nfds = std::max(nfds, *_writeMonitered.rbegin());
		if (!_exceptMonitered.empty())
			nfds = std::max(nfds, *_exceptMonitered.rbegin());
		
		//set the copy of monitered fd
		_readfdsCopy = _readfds;
		_writefdsCopy = _writefds;
		_exceptfdsCopy = _exceptfds;

		//if milliseconds is equal to zero, i.e no timeout
		if (!milliseconds)
			ret = ::select(nfds, &_readfdsCopy, &_writefdsCopy, &_exceptfdsCopy, 0);
		
		//if milliseconds is different than zero
		else
		{
			//set timeout value
			timeout.tv_sec = milliseconds / 1000;
			timeout.tv_usec = (milliseconds % 1000) * 1000;
			ret = ::select(nfds, &_readfdsCopy, &_writefdsCopy, &_exceptfdsCopy, &timeout);
		}
		if (ret < 0)
			throw std::runtime_error("select fecthing failure");
		return _checkFetchedFds(_readfdsCopy, _writefdsCopy, _exceptfdsCopy);
	}

	std::pair<int, Action>	Selecter::_checkFetchedFds(fd_set readfds, fd_set writefds, fd_set exceptfds) const
	{
		int	ret;

		ret = _checkFetchRead(readfds);
		if (ret >= 0)
			return std::make_pair(ret, aRead);
		ret = _checkFetchWrite(writefds);
		if (ret >= 0)
			return std::make_pair(ret, aWrite);
		ret = _checkFetchExcept(exceptfds);
		if (ret >= 0)
			return std::make_pair(ret, aExcept);
		return std::make_pair(ret, aAll);
	}

	int	Selecter::_checkFetchRead(fd_set readfds) const
	{
		std::set<int>::const_iterator	it;

		it = _readMonitered.begin();
		while (it != _readMonitered.end())
		{
			if (FD_ISSET(*it, &readfds))
				return *it;
		}
		return -1;
	}

	int	Selecter::_checkFetchWrite(fd_set writefds) const
	{
		std::set<int>::const_iterator	it;

		it = _writeMonitered.begin();
		while (it != _writeMonitered.end())
		{
			if (FD_ISSET(*it, &writefds))
				return *it;
		}
		return -1;
	}

	int	Selecter::_checkFetchExcept(fd_set exceptfds) const
	{
		std::set<int>::const_iterator	it;

		it = _exceptMonitered.begin();
		while (it != _exceptMonitered.end())
		{
			if (FD_ISSET(*it, &exceptfds))
				return *it;
		}
		return -1;
	}

	void	Selecter::_deepCopy(const Selecter& src)
	{
		_readfds = src._readfds;
		_writefds = src._writefds;
		_exceptfds = src._exceptfds;
	}
}