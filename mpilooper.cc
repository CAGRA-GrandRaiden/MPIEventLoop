/* Copyright (c) 2016- Chris Sullivan */
/* See the file "LICENSE" for the full license governing this code. */

#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <string>

#include "EventLoop.hh"

using namespace std;

void SignalHandler(int s);
vector<string> GetInputFiles(const char* inputfile);

int main(int argc, char** argv) {

  // install the interupt handler, to prevent an MPI hayday
  struct sigaction InteruptHandler;
  InteruptHandler.sa_handler = SignalHandler;
  sigemptyset(&InteruptHandler.sa_mask);
  InteruptHandler.sa_flags = 0;
  sigaction(SIGINT, &InteruptHandler, NULL);

  MPI_Init(&argc, &argv);

  EventLoop analyzer(GetInputFiles("./inputfiles.dat"));
  if (argc > 1) {   analyzer.SetOutputPath(argv[1]);    }
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
