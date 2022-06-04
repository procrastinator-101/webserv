#ifndef HEADERFIELD_HPP
# define HEADERFIELD_HPP

#include <string>

namespace ft
{
	class HeaderField
	{
		//================================================================================================
		//	attributes
		//================================================================================================
		public:
			std::string	key;
			std::string	value;
		//================================================================================================
		//	attributes End
		//================================================================================================

		//================================================================================================
		//	destructors, constructors, and assignment operators
		//================================================================================================
		public:
			HeaderField();
			~HeaderField();

			HeaderField(const HeaderField& src);
			HeaderField(const std::string& key, const std::string& value);

			HeaderField	&operator=(const HeaderField& rop);
		//================================================================================================
		//	destructors, constructors, and assignment operators End
		//================================================================================================


		//================================================================================================
		//	Header operations
		//================================================================================================
		public:
		//================================================================================================
		//	Header operations End
		//================================================================================================

		//================================================================================================
		//	private methods
		//================================================================================================
		private:
			void	_deepCopy(const HeaderField& src);
		//================================================================================================
		//	private methods End
		//================================================================================================
	};
}

#endif