#ifndef ACTION_HPP
#define ACTION_HPP

namespace ft
{
	enum Action
	{
		aRead = 1,
		aWrite = aRead * 2,
		aExcept = aWrite * 2,
		aAll = aRead | aWrite | aExcept
	};
}

#endif