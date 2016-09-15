/* Copyright (c) 2016- Chris Sullivan */
/* See the file "LICENSE" for the full license governing this code. */

#include "MPILooper.hh"
#include "merger.hh"
#include <TObject.h>
#include <TROOT.h>
#include <TObject.h>
#include <TSystem.h>
#include <TFile.h>
#include <TChain.h>
#include <TTree.h>
//#include <TSelector.h>
#include <TH2.h>
#include <TH1.h>
#include <TMath.h>
#include <TStopwatch.h>
#include <TEnv.h>
#include <TCanvas.h>
#include <TCutG.h>
//#include <TSelectorList.h>
#include "TGRUTOptions.h"


MPILooper::MPILooper(vector<string> inputlist)
  : compiled_histograms(TGRUTOptions::Get()->CompiledHistogramFile())
{

  MPI_Comm_size(MPI_COMM_WORLD, &m_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
  m_chain = make_shared<TChain>("EventTree");

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
  {
    TPreserveGDirectory preserve;
    m_output = make_shared<TFile>(m_string.str().c_str(),"recreate");
    compiled_histograms.SetDefaultDirectory(m_output.get());
  }
}
void MPILooper::Setup() {
  TObjArray *array = m_chain->GetListOfBranches();
  for(int x=0;x<array->GetSize();x++) {
    TBranch *b = (TBranch*)array->At(x);
    if(b) {
      TClass *c = TClass::GetClass(b->GetName());
      if(c) {
        printf("Found  %s!\n",b->GetName());
        TDetector** det = new TDetector*;
        *det = NULL;
        det_map[c] = det;
        m_chain->SetBranchAddress(b->GetName(),det_map[c]);
      }
    }
  }
}
void MPILooper::Finalize(){
  TPreserveGDirectory preserve;
  if(m_output){
    m_output->cd();
  }
  compiled_histograms.Write();
  if(GValue::Size()) {
    GValue::Get()->Write();
  }
  if(TChannel::Size()) {
    TChannel::Get()->Write();
  }
  compiled_histograms.SetDefaultDirectory(NULL);
  m_output->Write();
  m_output->Close();
  m_output = 0;

  MPI_Barrier(MPI_COMM_WORLD);

  // mergeing step
  if (m_rank == 0) {
    std::string grutpath = getenv("GRUTSYS");
    stringstream cmd; cmd.str(""); cmd << grutpath+"/util/gadd_fast.py -f " << m_outputpath.c_str() << " " << m_path.c_str() << "*";
    std::cout << cmd.str() << std::endl;
    system(cmd.str().c_str());
  }

}

void MPILooper::Run() {
  Setup();
  for (int i=m_lowerbound; i<m_upperbound; i++) {
    if (m_rank == 0) {    loadBar(i, m_threadcount, 1000, 50);    }
    //Process(i);
    for(auto& elem : det_map){
      *elem.second = (TDetector*)elem.first->New();
    }
    m_chain->GetEntry(i);

    TUnpackedEvent* event = new TUnpackedEvent;
    for(auto& elem : det_map){
      TDetector* det = *elem.second;
      if(!det->TestBit(TDetector::kUnbuilt)){
        event->AddDetector(det);
      } else {
        delete det;
      }
    }
    compiled_histograms.Fill(*event);
    delete event;

    // DEBUG FOR TESTING
    //if (i>10000) break;

  }
  if (m_rank == 0) { cout << endl; }
  this->Finalize();
  MPI_Barrier(MPI_COMM_WORLD);
}

MPILooper::~MPILooper() {
  if (m_rank==0){m_string.str(""); m_string << "rm -rf " << m_path; system(m_string.str().c_str());}

  MPI_Finalize();
}
