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
 * \file Client.cpp
 * \date October 2011
 * \author Matthieu Dorier
 * \version 0.3
 * \see Client.hpp
 */
#include <string.h>
#include <string>
#include <stdlib.h>
#include <iostream>
#include <exception>

#include "common/Debug.hpp"
#include "common/ChunkHandle.hpp"
#include "common/ShmChunk.hpp"
#include "common/Message.hpp"
#include "client/Client.hpp"

namespace Damaris {
	
	Client::Client(const std::string & configfile, int32_t coreID)
	{
		/* creates the configuration object from the configuration file */
		DBG("Starting Damaris client");
		std::auto_ptr<Model::simulation_mdl> 
			mdl(Model::simulation(configfile.c_str(),
						xml_schema::flags::dont_validate));

		DBG("Model initialized successfuly");

		Configuration::initialize(mdl,configfile);
		config = Configuration::getInstance();
		DBG("Configuration intialized successfuly");

		Environment::initialize(mdl,coreID);
		env = Environment::getInstance();
		DBG("Environment initialized succesfuly");
		init(config,env);
	}

	Client::Client(Configuration* config, Environment* env)
	{
		init(config,env);
	}

	void Client::init(Configuration* config, Environment* env)
	{
		id = env->getID();

		metadataManager = config->getMetadataManager();
		ASSERT(metadataManager != NULL);
		
		actionsManager = config->getActionsManager();
		ASSERT(metadataManager != NULL);
		/* initializes the shared structures */
		try {
#ifdef __SYSV
			msgQueue = SharedMessageQueue::open(sysv_shmem,
					config->getMsgQueueName().c_str());
			segment = SharedMemorySegment::open(sysv_shmem,
					config->getSegmentName().c_str());
#else
			msgQueue = SharedMessageQueue::open(posix_shmem,
					config->getMsgQueueName().c_str());
			segment = SharedMemorySegment::open(posix_shmem,
					config->getSegmentName().c_str());
#endif
			DBG("Client initialized successfully for core " << id 
			    << " with configuration \"" << config->getFileName() << "\"");
		}
		catch(interprocess_exception &ex) {
			ERROR("While initializing shared memory objects:  " << ex.what());
			exit(-1);
		}

		metadataManager = config->getMetadataManager();
		actionsManager = config->getActionsManager();
	}
	
	void* Client::alloc(const std::string & varname, int32_t iteration)
	{

		/* check that the variable is known in the configuration */
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
			return NULL;
        	}

		/* the variable is known, get its layout */
		Layout* layout = variable->getLayout();

		/* prepare variable to initialize a chunk */
		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
		for(unsigned int i=0; i < layout->getDimensions(); i++)	{
			ei[i] = layout->getExtentAlongDimension(i)-1;
			si[i] = 0;
		}

		/* try initializing the chunk in shared memory */
		try {
			ShmChunk* chunk = 
				new ShmChunk(segment,layout->getType(),
						layout->getDimensions(),si,ei);
			chunk->setSource(id);
			chunk->setIteration(iteration);
			variable->attachChunk(chunk);
			/* chunk initialized, returns the data! */
			return chunk->data();

		} catch (...) {
			ERROR("While allocating \"" << varname 
				<< "\", allocation failed");
		}
		/* on failure, returns NULL */
		return NULL;
	}
	
	int Client::commit(const std::string & varname, int32_t iteration)
	{		
		Variable* v = metadataManager->getVariable(varname);
		if(v == NULL)
			return -1;

		ShmChunk* chunk = NULL;
		// get the pointer to the allocated chunk
		ChunkIndexByIteration::iterator end;
		ChunkIndexByIteration::iterator it = v->getChunksByIteration(iteration,end);

		if(it == end)
			return -2;
		try {
			chunk = dynamic_cast<ShmChunk*>(it->get());
		} catch(std::exception &e) {
			ERROR("When doing dynamic cast: " << e.what());
			return -3;
		}
		
		// we don't need to keep the chunk in the client now,
		// so we erase it from the variable.
		v->eraseChunk(it);

		// create notification message
		Message message;
		message.source = id;

		message.iteration = iteration;
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		message.object = v->getID();
                // send message
		msgQueue->send(&message,sizeof(Message),0);
                // free message
		DBG("Variable \"" << varname << "\" has been commited");

		return 0;
	}
	
	int Client::write(const std::string & varname, int32_t iteration, const void* data)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname 
				<< "\" not defined in configuration");
			return -1;
        	}

		Layout* layout = variable->getLayout();

		std::vector<int> si(layout->getDimensions()),ei(layout->getDimensions());
                for(unsigned int i=0; i < layout->getDimensions(); i++) {
                        ei[i] = layout->getExtentAlongDimension(i)-1;
                        si[i] = 0;
                }

		ShmChunk* chunk = NULL;
                try {
                        chunk = new ShmChunk(segment,layout->getType(),
						layout->getDimensions(),si,ei);
                        chunk->setSource(id);
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
		
		// create message
		Message message;
		
		message.source = id;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		
		// send message
		msgQueue->send(&message,sizeof(Message),0);
		DBG("Variable \"" << varname << "\" has been written");
	
		delete chunk;
		return size;
	}

	int Client::chunk_write(chunk_h chunkh, const std::string & varname, 
			int32_t iteration, const void* data)
	{
		/* check that the variable is know in the configuration */
		Variable* variable = metadataManager->getVariable(varname);

        	if(variable == NULL) {
			ERROR("Variable \""<< varname << "\" not defined in configuration");
			return -1;
        	}

		ChunkHandle* chunkHandle = (ChunkHandle*)chunkh;

		/* check if the chunk matches the layout boundaries */
		Layout* layout = variable->getLayout();
		if(not chunkHandle->within(layout)) {
			ERROR("Chunk boundaries do not match variable's layout");
			return -3;
		}

		ShmChunk* chunk = NULL;
                try {
			Types::basic_type_e t = layout->getType();
			unsigned int d = chunkHandle->getDimensions();
			std::vector<int> si(d);
			std::vector<int> ei(d);

			for(unsigned int i=0;i<d; i++) {
				si[i] = chunkHandle->getStartIndex(i);
				ei[i] = chunkHandle->getEndIndex(i);
			}

                        chunk = new ShmChunk(segment,t,d,si,ei);
                        chunk->setSource(id);
                        chunk->setIteration(iteration);
                } catch (...) {
                        ERROR("While writing \"" << varname << "\", allocation failed");
                	return -2;
		}

		// copy data
		size_t size = chunk->getDataMemoryLength();
		memcpy(chunk->data(),data,size);
		
		// create message
		Message message;
		
		message.source = id;
		message.iteration = iteration;
		message.object = variable->getID();
		message.type = MSG_VAR;
		message.handle = chunk->getHandle();
		
		// send message
		msgQueue->send(&message,sizeof(Message),0);
		DBG("Variable \"" << varname << "\" has been written");
	
		// free message	
		delete chunk;
		
		return size;
	}

	int Client::signal(const std::string & signal_name, int32_t iteration)
	{
		Action* action = actionsManager->getAction(signal_name);
		if(action == NULL) {
			DBG("Undefined action \"" << signal_name << "\"");
			return -2;
		}

		Message sig;
		sig.source = id;
		sig.iteration = iteration;
		sig.type = MSG_SIG;
		sig.handle = 0;
		sig.object = action->getID();
		
		try {
			msgQueue->send(&sig,sizeof(Message),0);
		} catch(interprocess_exception &e) {
			ERROR("Error while sending event \"" << signal_name << "\", " << e.what());
			return -1;
		}
		DBG("Event \""<< signal_name << "\" has been sent");
		return 0;
	}

	int Client::get_parameter(const std::string & paramName, void* buffer)
	{
//		config->getParameterSet()
		// TODO
		return -1;
	}
		
	int Client::kill_server()
	{
		static int killed;
		if(!killed) {
			Message kill;
			kill.type = MSG_INT;
			kill.source = id;
			kill.iteration = -1;
			kill.object = KILL_SERVER;
			msgQueue->send(&kill,sizeof(Message),0);
			return 0;
		} else {
			WARN("Trying to send kill signal multiple times to the server");
			return -1;
		}
	}

	chunk_h Client::chunk_set(unsigned int dimensions,
			const std::vector<int> & startIndices, 
			const std::vector<int> & endIndices)
	{
		Types::basic_type_e t = Types::UNDEFINED_TYPE;
		ChunkHandle *c = new ChunkHandle(t,dimensions,startIndices,endIndices);
		return c;
	}

	void Client::chunk_free(chunk_h chunkh) 
	{
		if(chunkh != 0) delete (ChunkHandle*)chunkh;
	}
	
	Client::~Client() 
	{
		delete msgQueue;
		delete segment;

		config->finalize();
		env->finalize();

		INFO("Client destroyed successfuly");
	}
	
}

