#include "EventLoop.hh"

void EventLoop::Process(const int& entry) {
  m_chain->GetEntry(entry);

  // user defined analysis

}

EventLoop::EventLoop(vector<string> inputs)
  : MPILooper(inputs) { Setup(); }

EventLoop::~EventLoop(){

  // user defined cleanup (delete EventLoop heap members)

}

void EventLoop::Setup() {

  // user defined initialization

  TObjArray *fileElements=m_chain->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;
  int FileSuccessCount=0;


  while (( chEl=(TChainElement*)next() )) {
    TFile f(chEl->GetTitle());

    if (f.IsOpen()){
      FileSuccessCount++;
    }else {
      cout<<"Warning could not open "<<chEl->GetTitle()<<endl;
    }

    // user defined settings extraction from rootfiles

  }


  if ( FileSuccessCount == 0 ){
    cout<<"No files were successfully opened"<<endl;
    exit(0);
  }

  // associate userdefined event object with TChain, e.g.
  /** m_chain->SetBranchAddress("event",&event); **/

}
