#ifndef SELECTER_HPP
#define SELECTER_HPP

#include <set>
#include <map>
#include <utility>
#include <stdexcept>
#include <exception>

#include <sys/select.h>
// #include <sys/_types/_fd_def.h>

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
			void	add(int fd, int options);
			void	del(int fd, int options);

			std::map<int, int>	fetch(unsigned long milliseconds = 0) const;
		//================================================================================================
		//	Selecter operations End
		//================================================================================================


		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const Selecter& src);

			std::map<int, int>	_checkFetchedFds(fd_set readfds, fd_set writefds, fd_set exceptfds) const;
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif