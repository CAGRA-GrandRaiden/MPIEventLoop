#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

#include "TUnixSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TH1F.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TCollection.h"
#include "TChainElement.h"
#include "TRandom3.h"

#include "mpi.h"

using namespace std;

class MPILooper {
public:
  MPILooper(vector<string>);
  virtual ~MPILooper();

  virtual void Setup() = 0;
  void SetOutputPath(char* outpath) {m_outputpath = outpath;}
  
private:
  inline virtual void Process(const int& entry) = 0;
  virtual void Run();
  void Finalize();

  // MPI variables
  int m_rank;
  int m_size;
  int m_nentries;
  int m_threadcount;

  // IO variables
  const string m_path = "./tmp/";
  string m_tmpfile;
  stringstream m_string;
  string m_outputpath = "./output.root";

protected:
  int m_lowerbound;
  int m_upperbound;
  shared_ptr<TFile> m_output;
  shared_ptr<TChain> m_chain;
};
