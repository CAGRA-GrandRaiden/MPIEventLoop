#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "EventLoop.hh"

using namespace std;

void SignalHandler(int s) {
  int size; MPI_Comm_size(MPI_COMM_WORLD, &size);
  cout << "Caught signal: "<< s << ". Shutting down.." << endl;
  if (size > 1) {
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
  }
  exit(1);
}

int main(int argc, char** argv) {
  // install the interupt handler, to prevent an MPI hay day
  struct sigaction InteruptHandler;
  InteruptHandler.sa_handler = SignalHandler;
  sigemptyset(&InteruptHandler.sa_mask);
  InteruptHandler.sa_flags = 0;
  sigaction(SIGINT, &InteruptHandler, NULL);

  MPI_Init(&argc, &argv);

  EventLoop analyzer({
    "/user/sullivan/ceclub/sullivan/CESim/output??.root"
  });
  if (argc > 1) {   analyzer.SetOutputPath(argv[1]);    }
  analyzer.Run();

  return 0;
}
