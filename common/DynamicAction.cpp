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
 * \file DynamicAction.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "common/Debug.hpp"
#include "common/DynamicAction.hpp"

#include <unistd.h>

namespace Damaris {

	DynamicAction::DynamicAction(std::string fun, std::string file)
	: Action()
	{
		funName 	= fun;
		fileName	= file;
		handle 		= NULL;
		function	= NULL;
		loaded		= false;
	}
	
	DynamicAction::DynamicAction(std::string n, int i, std::string fun, std::string file)
	: Action(n,i)
	{
		funName 	= fun;
		fileName 	= file;
		handle 		= NULL;
		function 	= NULL;
		loaded		= false;
	}

	DynamicAction::~DynamicAction()
	{
		dlclose(handle);
	}
	
	void DynamicAction::call(int32_t iteration, int32_t sourceID)
	{
		if(!loaded)
			load();
		if(function != NULL)
			(*function)(name,iteration,sourceID);
                else
                  ERROR("Tryed to call function that does not exist!\n");
	}

	void DynamicAction::load()
	{
		if(loaded) return;

		char* error;
		void* handle = dlopen(fileName.c_str(),RTLD_NOW | RTLD_GLOBAL);
		
		if(!handle)
		{
		      char cwd[256];
		      getcwd(cwd,256);
			ERROR("While loading plugin in \"" << fileName.c_str() 
					<< "\":" << dlerror() << " (current dir is " << cwd << ")");
			return;
		}

		/* loading function */
		function = (void (*)(std::string,int32_t, int32_t))
			dlsym(handle,funName.c_str());

		if ((error = dlerror()) != NULL)  {
			ERROR("While loading function in dynamic library: " << error);
			return;
		}

                if (!function) {
                  ERROR("Could not find the function" << funName << "!\n");
                }

                INFO("Loaded function" << funName << std::endl);
	}
}
