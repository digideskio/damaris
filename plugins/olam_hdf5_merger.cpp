#include "hdf5.h"
#include <stdio.h>
#include "server/ServerConfiguration.hpp"
#include "common/MetadataManager.hpp"

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
size_t get_unidimensional_variable(
    Damaris::MetadataManager * db,
    const std::string * vname, 
    const int source_id,
    const int iteration_id,
    void * dest,
    size_t dest_size) 
{

  int any;
  Damaris::ChunkIndex::iterator it, end;
  Damaris::Variable * var = db->getVariable(vname);

  if (var == NULL) {
    return -1;
  }



  // Get an iterator to a chunk that matches source or iteration -- or both
  if  (  (source_id == -1) && (iteration_id == -1)  ) {
    it = var->getChunks(end);
  } else if ( (source_id >= 0) && (iteration_id >= 0) ) {
    it = var->getChunks( source_id, iteration_id, end);
  } else if (source_id >= 0) {
    it = var->getChunksBySource(source_id, end);
  } else {   // if (iteration_id >= 0)
    it = var->getChunksByIteration(iteration_id, end);
  }

  //
  while (it != end) {
    Damaris::Chunk * chunk;
    Damaris::Types::basic_type_e var_type ;
    size_t elementary_type_size;

    chunk     = it->get();
    var_type  = chunk->getType();
    elementary_type_size = Damaris::Types::basicTypeSize(type);


    // Make sure that we can store the first (ideally only) element of the
    // array into dest
    if (elementary_type_size > dest_size) {
      cerr << "warning: Trying to get " << dest_size << " bytes of variable " << vname << " but elementary type has " << elementary_type_size << "bytes!!!!"  << endl;

      return -1;
    }

    memcpy(dest, chunk->data(), elementary_type_size) ;
    return elementary_type_size;
  }

  // if there was no valid chunk, default to -1
  return -1;



}



hid_t get_hdf5_type(const Damaris::Types::basic_type_e etype)
{
  switch(etype) {
    case SHORT:     //  short int, integer*2

      return  H5T_NATIVE_SHORT;


    case INT:       // int, integer*4

      return H5T_NATIVE_INT;

    case LONG:      // long int, integer*8

      return H5T_NATIVE_LONG;

    case FLOAT:     // float, real 

      return H5T_NATIVE_FLOAT;

    case DOUBLE:    // double, real*8

      return H5T_NATIVE_DOUBLE;

    case CHAR:      // char, character

      return H5T_NATIVE_CHAR;

    case STR:       // string 

      return H5T_C_S1;

    case UNDEFINED_TYPE:  /*!< don't know the type  (bad...) */

      cerr << "Unknown datatype!!! Defaults to char" << endl;
      break;
  }

  return H5T_NATIVE_CHAR;


}








  void dump_hdf5(const std::string * event, int32_t step, int32_t src,
                 Damaris::MetadataManager * db) {
    Damaris::ServerConfiguration * config =
        Damaris::ServerConfiguration::getInstance();
    int nb_clients = config->getClientsPerNode();
    static int waiting;
     waiting++;

    if (waiting == nb_clients) {
      waiting = 0;
      //TIMER_START(write_time) 
      hid_t dataset_id, dataspace_id, chunk_id;
      hid_t file_id;
      hid_t link_prop_id;

      char filename[128];
      char dsetname[128];

      unsigned int gzip_filter_values[1];
       gzip_filter_values[0] = 4;

      int serverID = config->getServerID();
      // create the file
       sprintf(filename, "olam_hist.%d.%d.h5", (int) step, serverID);

       file_id =
          H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

       link_prop_id = H5Pcreate(H5P_LINK_CREATE);
       H5Pset_create_intermediate_group(link_prop_id, 1);

      // olam has at most 2 dimensions, but just in case, I'll
      // keep the 3
      hsize_t dims[3], chunkdims[3];

       Damaris::Variable * v;

       std::list < Damaris::Variable > *lv = db->getAllVariables();
       std::list < Damaris::Variable >::iterator i;

      for (i = lv->begin(); i != lv->end();) {
        v = &(*i);
        Damaris::Layout * ly = v->layout;

        //if(v->getIteration() == step)
        if (v != NULL) {
          switch (ly->getDimensions()) {
          case 0:
            break;

          case 1:
            break;
            
          case 2:
            chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
            chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);

            dataspace_id =
                H5Screate_simple(ly->getDimensions(), dims, NULL);

            if (v->name.c_str()[0] == '/')
              sprintf(dsetname, "%d%s", v->source, v->name.c_str());
            else
              sprintf(dsetname, "%d/%s", v->source, v->name.c_str());

            dataset_id =
                H5Dcreate(file_id, dsetname, H5T_NATIVE_FLOAT,
                          dataspace_id, link_prop_id, H5P_DEFAULT,
                          H5P_DEFAULT);

            H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, v->data);

            H5Dclose(dataset_id);
           
            break;
            
          case 3:
            chunkdims[0] = dims[0] = ly->getExtentAlongDimension(0);
            chunkdims[1] = dims[1] = ly->getExtentAlongDimension(1);
            chunkdims[2] = dims[2] = ly->getExtentAlongDimension(2);

            chunk_id = H5Pcreate(H5P_DATASET_CREATE);
            H5Pset_chunk(chunk_id, ly->getDimensions(), chunkdims);
#ifdef __ENABLE_COMPRESSION
            H5Pset_filter(chunk_id, 1, 0, 1, gzip_filter_values);
#endif
            dataspace_id =
                H5Screate_simple(ly->getDimensions(), dims, NULL);
            if (v->name.c_str()[0] == '/')
              sprintf(dsetname, "%d%s", v->source, v->name.c_str());
            else
              sprintf(dsetname, "%d/%s", v->source, v->name.c_str());
            dataset_id =
                H5Dcreate(file_id, dsetname, H5T_NATIVE_FLOAT,
                          dataspace_id, link_prop_id, chunk_id,
                          H5P_DEFAULT);
            H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
                     H5P_DEFAULT, v->data);
            H5Dclose(dataset_id);
            break;

          } i++;
          db->remove(*v);
          // end for each
        }                       // if
      }                         // for 

      H5Fclose(file_id);
      H5Pclose(link_prop_id);
      H5close();
    }
  }

}
