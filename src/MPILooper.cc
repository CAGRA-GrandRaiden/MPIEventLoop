#include "MPILooper.hh"
#include "merger.hh"
#include <TObject.h>
#include <TROOT.h>
#include <TObject.h>
#include <TSystem.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
#include <TSelector.h>
#include <TH2.h>
#include <TH1.h>
#include <TMath.h>
#include <TStopwatch.h>
#include <TEnv.h>
#include <TCanvas.h>
#include <TCutG.h>
#include <TSelectorList.h>


MPILooper::MPILooper(vector<string> inputlist)
  : m_selector(new TSelectorList()) {

  MPI_Comm_size(MPI_COMM_WORLD, &m_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
  m_chain = make_shared<TChain>("simtree");

  // add all input files to the TChain
  for(auto fn : inputlist) {
    m_chain->Add(fn.c_str());
  }

  // divide the events up into equal chunks
  m_nentries = m_chain->GetEntries();
  m_threadcount = m_nentries/m_size;
  m_lowerbound = 0;
  int remainder = m_nentries - m_threadcount*m_size;

  // distribute remainder of events evenly
  if (m_rank < remainder) {
    m_threadcount++;
    m_lowerbound = m_rank*m_threadcount;
  } else {
    m_lowerbound = remainder*(m_threadcount+1)+(m_rank-remainder)*m_threadcount;
  }
  m_upperbound = m_lowerbound+m_threadcount;
  cout << "Rank: "<<m_rank<< " EventRange: ["<<m_lowerbound<< ","<<m_upperbound<<"]"<<endl;

  // temporary file directory
  m_tmpfile = "0_"+to_string(m_rank);
  m_string.str(""); m_string << m_path << m_tmpfile << ".root";

  if (m_rank==0) {gSystem->MakeDirectory(m_path.c_str());}
  MPI_Barrier(MPI_COMM_WORLD);
  // open temporary output file
  m_output = make_shared<TFile>(m_string.str().c_str(),"recreate");
}

void MPILooper::Finalize(){
  // ROOT shinanigans to properly write
  // the TSelectorList to file
  TDirectory* prev = gDirectory;
  m_output->cd();
  m_selector->Write();
  prev->cd();
  m_output->Write();
  m_output->Close();

  MPI_Barrier(MPI_COMM_WORLD);
  mt_binarytree_merge(m_outputpath.c_str(),m_path.c_str(),m_size/2,m_rank,m_size);
}

void MPILooper::Run() {

  for (int i=m_lowerbound; i<m_upperbound; i++) {
    if (m_rank == 0) {    loadBar(i, m_threadcount, 1000, 50);    }
    Process(i);
  }
  if (m_rank == 0) { cout << endl; }
  this->Finalize();
  MPI_Barrier(MPI_COMM_WORLD);
}

MPILooper::~MPILooper() {
  if (m_rank==0){m_string.str(""); m_string << "rm -rf " << m_path; system(m_string.str().c_str());}

  delete m_selector;
  MPI_Finalize();
}



/**Make a 1D histogram with a Name

 */
void MPILooper::MakeHistogram(TString name,Int_t bins,Double_t xlow,Double_t xhigh){
  TH1F* h=new TH1F(name,name,bins,xlow,xhigh);
  m_selector->AddLast(h);
  gDirectory->Add(h);
}

/**Fill a 1D histogram with a name

 */
void MPILooper::FillHistogram(TString name,Float_t value){

  TObject * object = m_selector->FindObject(name);

  if (object == NULL){
    Error("FillHistogram",name+" not found");
    return;
  }
  TString className=object->ClassName();
  if (className !="TH1F"){
    Error("FillHistogram",name+" not a histogram");
  }

  ((TH1F*)object)->Fill(value);

}


/**Makea 2D histogram with a Name
 */
void MPILooper::MakeHistogram(TString name,Int_t binsX,Double_t xlow,Double_t xhigh,Int_t binsY,Double_t yLow,Double_t yHigh){
  TH2F* h=new TH2F(name,name,binsX,xlow,xhigh,binsY,yLow,yHigh);
  m_selector->AddLast(h);
  gDirectory->Add(h);
}


/**Fill a 2D histogram with a Name
 */
void MPILooper::FillHistogram(TString name,Float_t Xvalue,Float_t Yvalue){

  TObject * object = m_selector->FindObject(name);
  
  if (object == NULL){
    Error("FillHistogram",name+" not found");
    return;
  }
  TString className=object->ClassName();
  if (className !="TH2F"){
    Error("FillHistogram",name+" not a histogram");
  }

  ((TH2F*)object)->Fill(Xvalue,Yvalue);

}


void MPILooper::Hist(TString name,Float_t value,Int_t bins, Double_t xlow, Double_t xhigh){

  TObject * object = m_selector->FindObject(name);
  if ( object == NULL) {//The histogram is not there
    MakeHistogram(name,bins,xlow,xhigh);
  }
  FillHistogram(name,value);
}

void MPILooper::Hist(TString name,Float_t Xvalue,Float_t Yvalue,Int_t binsX, Double_t xlow, Double_t xhigh,Int_t binsY,Double_t yLow,Double_t yHigh){

  TObject * object = m_selector->FindObject(name);
  if ( object == NULL) {//The histogram is not there
    MakeHistogram(name,binsX,xlow,xhigh,binsY,yLow,yHigh);
  }
  FillHistogram(name,Xvalue,Yvalue);

}


void MPILooper::Hist(Int_t HistNumber,Float_t value,Int_t bins, Double_t xlow, Double_t xhigh){
  stringstream s;
  s<<"h"<<HistNumber;
  Hist(s.str().c_str(),value,bins,xlow,xhigh);

}
void MPILooper::Hist(Int_t HistNumber,Float_t Xvalue,Float_t Yvalue,Int_t binsX, Double_t xlow, Double_t xhigh,Int_t binsY,Double_t ylow,Double_t yhigh){
  stringstream s;
  s<<"h"<<HistNumber;
  Hist(s.str().c_str(),Xvalue,Yvalue,binsX,xlow,xhigh,binsY,ylow,yhigh);

}



