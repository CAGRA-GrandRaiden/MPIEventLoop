#include "EventLoop.hh"

using namespace std;

int main(int argc, char** argv) {
  
  MPI_Init(&argc, &argv);
  
  EventLoop analyzer({
    "/mnt/analysis/rcnp-e441/sims/segcorrect/output10.root",
    "/mnt/analysis/rcnp-e441/sims/segcorrect/output11.root"
  });
  

  
  
  return 0;

}
