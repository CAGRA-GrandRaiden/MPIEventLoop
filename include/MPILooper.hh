#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <sstream>

#include "TFile.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TSelectorList.h"
#include "TRandom3.h"

#include "mpi.h"

using namespace std;

class MPILooper {
public:
  MPILooper(vector<string>);
  virtual ~MPILooper();

  void Run();
  void SetOutputPath(char* outpath) {m_outputpath = outpath;}

private:
  virtual void Process(const int& entry) = 0;
  void Finalize();

protected:
  virtual void Setup() = 0;
  void MakeHistogram(TString name,Int_t bins,Double_t xlow,Double_t xhigh);
  void FillHistogram(TString name,Float_t value);
  void MakeHistogram(TString name,Int_t binsX,Double_t xlow,Double_t xhigh,Int_t binsY,Double_t yLow,Double_t yHigh);
  void FillHistogram(TString name,Float_t Xvalue,Float_t Yvalue);
  void Hist(TString name,Float_t value,Int_t bins, Double_t xlow, Double_t xhigh);
  void Hist(TString name,Float_t Xvalue,Float_t Yvalue,Int_t binsX, Double_t xlow, Double_t xhigh,Int_t binsY,Double_t yLow,Double_t yHigh);
  void Hist(Int_t HistNumber,Float_t value,Int_t bins, Double_t xlow, Double_t xhigh);
  void Hist(Int_t HistNumber,Float_t Xvalue,Float_t Yvalue,Int_t binsX, Double_t xlow, Double_t xhigh,Int_t binsY,Double_t yLow,Double_t yHigh);


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
  TSelectorList* m_selector;

};

#include "LoadingBar.hh"

