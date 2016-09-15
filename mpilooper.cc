/* Copyright (c) 2016- Chris Sullivan */
/* See the file "LICENSE" for the full license governing this code. */

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>

#include "MPILooper.hh"
#include "TGRUTOptions.h"
#include "TGRUTint.h"
#include "TDetectorEnv.h"
#include "TInverseMap.h"
#include "TChannel.h"
#include "GValue.h"


using namespace std;

void SignalHandler(int s);
vector<string> GetInputFiles(const char* inputfile);

int main(int argc, char** argv) {

  auto opt = TGRUTOptions::Get(argc, argv);
  //auto input = TGRUTint::instance(argc,argv);


  TDetectorEnv::Get(opt->DetectorEnvironment().c_str());

  if(opt->S800InverseMapFile().length()) {
    TInverseMap::Get(opt->S800InverseMapFile().c_str());
  }

  // for(auto filename : opt->RootInputFiles()) {
  //   // this will populate gChain if able.
  //   //   TChannels from the root file will be loaded as file is opened.
  //   //   GValues from the root file will be loaded as file is opened.
  //   OpenRootFile(filename);
  // }

  //if I am passed any calibrations, lets load those, this
  //will overwrite any with the same address previously read in.
  if(opt->CalInputFiles().size()) {
    for(auto cal_filename : opt->CalInputFiles()) {
      TChannel::ReadCalFile(cal_filename.c_str());
    }
  }
  if(opt->ValInputFiles().size()) {
    for(auto val_filename : opt->ValInputFiles()) {
      GValue::ReadValFile(val_filename.c_str());
    }
  }






  // install the interupt handler, to prevent an MPI hayday
  struct sigaction InteruptHandler;
  InteruptHandler.sa_handler = SignalHandler;
  sigemptyset(&InteruptHandler.sa_mask);
  InteruptHandler.sa_flags = 0;
  sigaction(SIGINT, &InteruptHandler, NULL);

  MPI_Init(&argc, &argv);

  MPILooper analyzer(GetInputFiles("./inputfiles.dat"));
  //if (argc > 1) {   analyzer.SetOutputPath(argv[1]);    }
  analyzer.Run();

  return 0;
}

void SignalHandler(int s) {
  int size; MPI_Comm_size(MPI_COMM_WORLD, &size);
  cout << "Caught signal: "<< s << ". Shutting down.." << endl;
  if (size > 1) {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
  }
  exit(1);
}

vector<string> GetInputFiles(const char* inputfile) {
  ifstream input(inputfile);
  string fileline;
  vector<string> inputfiles;
  while(input >> fileline) {
    if (fileline.size() > 0)inputfiles.push_back(fileline);
  }
  return inputfiles;
}
