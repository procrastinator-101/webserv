#ifndef SELECTER_HPP
#define SELECTER_HPP

#include <set>
#include <utility>
#include <sys/select.h>
#include <sys/_types/_fd_def.h>

#include "Action.hpp"

namespace ft
{
	class Selecter
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		private:
			fd_set	_readfds;
			fd_set	_writefds;
			fd_set	_exceptfds;
			std::set<int>	_readMonitered;
			std::set<int>	_writeMonitered;
			std::set<int>	_exceptMonitered;
		//================================================================================================
		//	attributes End
		//================================================================================================


		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			Selecter();
			~Selecter();

			Selecter(const Selecter& src);

			Selecter	&operator=(const Selecter& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Selecter operations
		//================================================================================================
		public:
			void	add(int fd, Action action);
			void	del(int fd, Action action);

			std::pair<int, Action>	fetch(unsigned long milliseconds = 0) const;
		//================================================================================================
		//	Selecter operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Selecter& src);

			int	_checkFetchRead(fd_set readfds) const;
			int	_checkFetchWrite(fd_set writefds) const;
			int	_checkFetchExcept(fd_set exceptfds) const;
			std::pair<int, Action>	_checkFetchedFds(fd_set readfds, fd_set writefds, fd_set exceptfds) const;
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif