// C++ source code
// File: "/home/kassick/Work/damaris.git/plugins/olam_hdf5_merger/test_hdf5.cpp"
// Created: "Ter, 27 Mar 2012 22:39:38 -0300 (kassick)"
// Updated: "Ter, 27 Mar 2012 23:15:36 -0300 (kassick)"
// $Id$
// Copyright (C) 2012, Rodrigo Virote Kassick <rvkassick@inf.ufrgs.br> 
/*
 * ===========================================================================
 *
 *       Filename:  test_hdf5.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  27-03-2012 22:39:38 BRT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (), 
 *        Company:  
 *
 * ==========================================================================
 */

//#define H5_USE_18_API 1
//#define H5_NO_DEPRECATED_SYMBOLS  1
#define H5Dcreate_vers 2
#include <hdf5.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{

  hid_t dataset_id, dataspace_id, chunk_id;
  hid_t file_id;
  hid_t link_prop_id;
  unsigned int gzip_filter_values[1];
  hsize_t dims[30], chunkdims[30];

  float mydata[64][16][4];
  int i, j, k, dimensions;




  for (i = 0; i < 64; i++) {
    for (j = 0; j < 16; j++) {
      for (k = 0; k < 4; k++) {
        mydata[i][j][k] = i * j * k;
      }
    }
  }

  file_id = H5Fcreate("test.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

  link_prop_id = H5Pcreate(H5P_LINK_CREATE);
  H5Pset_create_intermediate_group(link_prop_id, 1);

  gzip_filter_values[0] = 4;

  dimensions = 3;
  dims[0] = 64;
  dims[1] = 16;
  dims[2] = 4;


  dataspace_id = H5Screate_simple(dimensions, dims, NULL);
  if (dataspace_id <= 0) {
    printf("dataspace_id =  %d\n", dataset_id);
    exit(1);
  }

  dataset_id =
      H5Dcreate(file_id,
                "/myvar", H5T_NATIVE_FLOAT, dataspace_id, link_prop_id,
                H5P_DEFAULT, H5P_DEFAULT);

  if (dataset_id <= 0) {
    printf("no good\n");
    H5Eprint(stdout);
    exit(1);
  }

  //exit(1);
  H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL,
           H5P_DEFAULT, mydata);


  H5Dclose(dataset_id);
  H5Fclose(file_id);
  H5Pclose(link_prop_id);
  H5close();

}
