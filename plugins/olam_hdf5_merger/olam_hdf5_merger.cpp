#include "hdf5.h"
#include <stdio.h>
//#include "server/ServerConfiguration.hpp"
#include "common/MetadataManager.hpp"
#include "common/Types.hpp"
#include "common/Configuration.hpp"
#include "common/Environment.hpp"

using namespace std;

extern "C" {


  /*

     HDF5_Type get_type_of_var(Damaris::Variable &v) {
     }

   */


/*****
 * Extract the first element of a variable
 * with source or iteration = -1 , select any occurence of the var
 * */
  size_t get_unidimensional_variable(Damaris::MetadataManager * db,
                                     const std::string & vname,
                                     const int source_id,
                                     const int iteration_id,
                                     void *dest, size_t dest_size) {

    int any;
     Damaris::Variable * var = db->getVariable(vname);
     Damaris::Chunk * chunk;

    if (var == NULL) {
      return -1;
    }

    chunk = NULL;



    // Get an iterator to a chunk that matches source or iteration -- or both
    if ((source_id == -1) && (iteration_id == -1)) {

      Damaris::ChunkIndex::iterator it, end;

      it = var->getChunks(end);

      if (it != end) {
        chunk = it->get();
      }
    } else if ((source_id >= 0) && (iteration_id >= 0)) {

      Damaris::ChunkIndex::iterator it, end;

      it = var->getChunks(source_id, iteration_id, end);
      it = var->getChunks(end);

      if (it != end) {
        chunk = it->get();
      }
    } else if (source_id >= 0) {

      Damaris::ChunkIndexBySource::iterator it, end;

      it = var->getChunksBySource(source_id, end);
      if (it != end) {
        chunk = it->get();
      }

    } else {                    // if (iteration_id >= 0)

      Damaris::ChunkIndexByIteration::iterator it, end;

      it = var->getChunksByIteration(iteration_id, end);
      if (it != end) {
        chunk = it->get();
      }

    }

    //
    if (chunk) {
      Damaris::Types::basic_type_e var_type;
      size_t elementary_type_size;

      var_type = chunk->getType();
      elementary_type_size = Damaris::Types::basicTypeSize(var_type);


      // Make sure that we can store the first (ideally only) element of the
      // array into dest
      if (elementary_type_size > dest_size) {
        cerr << "warning: Trying to get " << dest_size <<
            " bytes of variable " << vname << " but elementary type has "
            << elementary_type_size << "bytes!!!!" << endl;

        return -1;
      }

      memcpy(dest, chunk->data(), elementary_type_size);
      return elementary_type_size;
    }
    // if there was no valid chunk, default to -1
    return -1;



  }



  hid_t get_hdf5_type(const Damaris::Types::basic_type_e etype) {
    switch (etype) {
    case Damaris::Types::SHORT:        //  short int, integer*2

      return H5T_NATIVE_SHORT;


    case Damaris::Types::INT:  // int, integer*4

      return H5T_NATIVE_INT;

    case Damaris::Types::LONG: // long int, integer*8

      return H5T_NATIVE_LONG;

    case Damaris::Types::FLOAT:        // float, real 

      return H5T_NATIVE_FLOAT;

    case Damaris::Types::DOUBLE:       // double, real*8

      return H5T_NATIVE_DOUBLE;

    case Damaris::Types::CHAR: // char, character

      return H5T_NATIVE_CHAR;

    case Damaris::Types::STR:  // string 

      return H5T_C_S1;

    case Damaris::Types::UNDEFINED_TYPE:       /*!< don't know the type  (bad...) */

      cerr << "Unknown datatype!!! Defaults to char" << endl;
      break;
    }

    return H5T_NATIVE_CHAR;


  }








  void dump_hdf5(const std::string * event, int32_t step, int32_t src,
                 Damaris::MetadataManager * db) {

    //Damaris::Configuration * config;
    Damaris::Environment * env;

    cerr << "Hello there!!!" << endl;


    //config = Damaris::Configuration::getInstance();
    env = Damaris::Environment::getInstance();

    int nb_clients = env->getClientsPerNode();
    static int waiting;
    waiting++;

    //if (waiting == nb_clients)
    {
      waiting = 0;
      //TIMER_START(write_time) 
      hid_t dataset_id, dataspace_id, chunk_id;
      hid_t file_id;
      hid_t link_prop_id;

      char filename[128];
      char dsetname[128];

      unsigned int gzip_filter_values[1];
      gzip_filter_values[0] = 4;

      int serverID = env->getID();
      // create the file
      sprintf(filename, "olam_hist.%d.%d.h5", (int) step, serverID);

      cerr << "Will create " << filename << endl;

      file_id =
          H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

      link_prop_id = H5Pcreate(H5P_LINK_CREATE);
      H5Pset_create_intermediate_group(link_prop_id, 1);

      // olam has at most 2 dimensions, but just in case, I'll
      // keep the 3
      hsize_t dims[30], chunkdims[30];


      Damaris::VariableSet & lv = db->getVariableSet();
      Damaris::VariableSet::iterator i;

      for (i = lv.begin(); i != lv.end();) {
        Damaris::Variable * v = i->get();


        Damaris::ChunkIndex::iterator chunk_it, chunk_end;

        chunk_it = v->getChunks(chunk_end);

        while (chunk_it != chunk_end) {

          Damaris::Chunk * c = chunk_it->get();

          if (c->getIteration() == step) {


            int dimensions = c->getDimensions();
            for (int i = 0; i < dimensions; i++) {
              dims[i] = chunkdims[i] =
                  c->getEndIndex(i) - c->getStartIndex(i) + 1;
            }

            if (dimensions >= 3)        // is it really necessary?
            {
              chunk_id = H5Pcreate(H5P_DATASET_CREATE);
              H5Pset_chunk(chunk_id, dimensions, chunkdims);

#ifdef __ENABLE_COMPRESSION
              H5Pset_filter(chunk_id, 1, 0, 1, gzip_filter_values);
#endif

            } else {
              chunk_id = H5P_DEFAULT;
            }


            dataspace_id = H5Screate_simple(dimensions, dims, NULL);

            sprintf(dsetname, "%d/%s", c->getSource(), v->getName().c_str());


            dataset_id =
                H5Dcreate(file_id,
                          dsetname,
                          get_hdf5_type(c->getType()),
                          dataspace_id,
                          link_prop_id);
                          //H5P_DEFAULT,
                          //H5P_DEFAULT);

            H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, c->data());

            H5Dclose(dataset_id);
          }                     // correct iteration?



          chunk_it++;

        }
        v->clear();             // clear all chunks!!!!!
      }                         // for 

      H5Fclose(file_id);
      H5Pclose(link_prop_id);
      H5close();
    }

  }

}
