/* Copyright (c) 2016- Chris Sullivan */
/* See the file "LICENSE" for the full license governing this code. */

#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>


using namespace std;

int merge(const char* path, int outputidx, int level, int* files, size_t size) {

  stringstream cmd; cmd.str(""); cmd << "hadd -f ";
  cmd << path << "/" << level+1 << "_" << outputidx << ".root ";
  for (uint32_t i=0; i<size; i++) {
    cmd << path << "/" << level << "_" << files[i] << ".root ";
  }
  //cout <<"Rank: "<<outputidx << " - " << cmd.str() <<endl;
  system(cmd.str().c_str());
  return 0;
}
int mt_binarytree_merge(const char* outputfile, const char* path, int nmergers, const int& rank, const int& mpisize) {
  if (mpisize <= 1) {
    stringstream cmd; cmd.str(""); cmd << "mv " << path <<"/0_0.root " << outputfile;
    system(cmd.str().c_str());
    return 0;
  }

  MPI_Group orig_group, new_group;
  MPI_Comm comm_mergers;

  /* Extract the original group handle */
  MPI_Comm_group(MPI_COMM_WORLD, &orig_group);

  if ( rank < nmergers) {

    // Create a new communicator with only a subset of ranks
    int* ranks = new int[nmergers];
    for (int i = 0; i< nmergers; i++) { ranks[i] = i; }

    // Form the MPI group of ranks
    MPI_Group_incl(orig_group, nmergers, ranks, &new_group);

    // Create the new communicator
    MPI_Comm_create(MPI_COMM_WORLD, new_group, &comm_mergers);

    int nfiles = mpisize;
    int nfilestomerge_perthread = 0;
    int level = 0;
    do {
      // only the first N ranks will participate in the merge processes
      // where N = nmergers (the number of merging processes).
      if ( rank < nmergers) {

        nfilestomerge_perthread = nfiles/nmergers;
        int remainder = nfiles - nfilestomerge_perthread*nmergers;

        // if there is a remainder, add it to the last threads tasks
        if (remainder != 0 && rank == nmergers-1) {

          int *files = new int[nfilestomerge_perthread+remainder];
          int idxfile = 0;
          for (int i=nfilestomerge_perthread*rank; i < nfilestomerge_perthread*(rank+1)+remainder; i++) {
            files[idxfile]=i;
            idxfile++;
          }
          merge(path,rank,level,files,nfilestomerge_perthread);

          delete[] files;
        } else {

          int *files = new int[nfilestomerge_perthread];
          int idxfile = 0;
          for (int i=nfilestomerge_perthread*rank; i < nfilestomerge_perthread*(rank+1); i++) {
            files[idxfile]=i;
            idxfile++;
          }
          merge(path,rank,level,files,nfilestomerge_perthread);
          delete[] files;
        }

        nfiles = nmergers;
        nmergers /= 2;
        level++;

      } else { break; }

      // Wait until all merging threads are complete with the current level
      // TODO: performance could be improved by using a master-worker model
      // for the tree merge
      MPI_Barrier(comm_mergers);
    } while (nmergers > 0);


    if (rank == 0) {
      stringstream cmd; cmd.str(""); cmd << "mv " << path <<"/"<<level-1<<"_0.root " << outputfile;
      system(cmd.str().c_str());
    }
  }

  return 0;
}
