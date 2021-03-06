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
 * \file NodeAction.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#include "common/Debug.hpp"
#include "common/NodeAction.hpp"

namespace Damaris {

	NodeAction::NodeAction(Action* a, int clpn)
	: Action()
	{
		base = a;
		clientsPerNode = clpn;
	}
	
	NodeAction::~NodeAction()
	{
		delete base;
	}
	
	void NodeAction::call(int32_t iteration, int32_t sourceID)
	{
		locks[iteration] = locks[iteration] + 1;
		if(locks[iteration] == clientsPerNode) {
			base->call(iteration,sourceID);
			locks.erase(iteration);
		}
	}
}
