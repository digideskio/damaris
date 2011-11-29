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
 * \file Options.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.1
 * 
 * The Options object parses the command line, creates an Environment
 * and a Configuration object that will be passed to the Server object.
 */
#ifndef __DAMARIS_OPTIONS_H
#define __DAMARIS_OPTIONS_H

#include <string>

#include "common/Configuration.hpp"
#include "common/Environment.hpp"

namespace Damaris {

/**
 * The Options object can be used to parse the command line,
 * it will prepare the Configuration and the Environment
 * objects. This object is used only with a standalone server,
 * not with an embedded one.
 */
class Options {
	private:
		std::string* configFile; /*!< Name of the configuration file. */
		Environment* env; /*!< Pointer to the Environment object that will be filled. */
		Configuration* config; /*!< Pointer to the Configuration 
					 that will be initialized. */
	public:
		/**
		 * The constructor takes the same parameters than a usual program. 
		 */
		Options(int argc, char** argv);
		/**
		 * Get the name of the configuration file.
		 */
		std::string* getConfigFile();
		/**
		 * Get the prepared configuration.
		 */
		Configuration* getConfiguration();
		/**
		 * Get the prepared environment.
		 */
		Environment* getEnvironment();
};
}

#endif
