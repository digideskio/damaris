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
 * \file SharedMemory.hpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 */
#ifndef __DAMARIS_SHARED_MEMORY_H
#define __DAMARIS_SHARED_MEMORY_H

#include <boost/interprocess/managed_shared_memory.hpp>

namespace Damaris {

	/**
	 * POSIX shared memory support (based on shm_open).
	 */
	struct posix_shmem_t { };
	/**
	 * System-V shared memory support (based on shmget).
	 */
	struct sysv_shmem_t { };

	static const struct posix_shmem_t posix_shmem = posix_shmem_t();
	static const struct sysv_shmem_t sysv_shmem = sysv_shmem_t();

	/**
	 * A handle_t is an object that can be converted into pointers
	 * even in different process memory spaces.
	 */
	typedef boost::interprocess::managed_shared_memory::handle_t handle_t;
}

#endif
