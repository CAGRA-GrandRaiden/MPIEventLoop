/* Copyright (c) 2016- Chris Sullivan */
/* See the file "LICENSE" for the full license governing this code. */

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <sstream>
#include <map>

#include "TFile.h"
#include "TChain.h"
#include "TChainElement.h"
//#include "TSelectorList.h"
#include "TRandom3.h"

#include "TCompiledHistograms.h"
#include "TGRUTint.h"
#include "TGRUTOptions.h"
#include "TPreserveGDirectory.h"
#include "GValue.h"
#include "TChannel.h"
#include "TDetector.h"
#include "TUnpackedEvent.h"

#include "mpi.h"

using namespace std;

class MPILooper {
public:
  MPILooper(vector<string>);
  virtual ~MPILooper();

  void Run();
  void SetOutputPath(char* outpath) {m_outputpath = outpath;}

private:
  //virtual void Process(const int& entry);
  void Finalize();

protected:
  virtual void Setup();


protected:
  int m_lowerbound;
  int m_upperbound;
  shared_ptr<TFile> m_output;
  shared_ptr<TChain> m_chain;

private:
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
  //TSelectorList* m_selector;

  TCompiledHistograms compiled_histograms;
  map<TClass*, TDetector**> det_map;

};

#include "LoadingBar.hh"

