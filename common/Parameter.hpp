/*******************************************************************
This file is part of Damaris.

Damaris is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Damaris is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Damaris.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/
/**
 * \file Parameter.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_PARAMETER_H
#define __DAMARIS_PARAMETER_H

#include <boost/any.hpp>
#include <string>

namespace Damaris {
	/**
	 * The Parameter class is based on boost::any to hold
	 * any type of data and associate it with a name.
	 */	
	class Parameter {
		private:
			std::string name; /*!< Name of the parameter. */

		public:
			boost::any value; /*!< Value of the parameter. */
		
			/**
			 * Constructor.
			 */
			template<typename T>
			Parameter(const std::string&, const T&);

			/**
			 * Gets the name of the parameter.
			 */
			std::string getName() const;
	};
}

#endif

namespace Damaris {

template<typename T>
Parameter::Parameter(const std::string& n, const T& v)
{
        name = n;
        value = boost::any(v);
}

}
