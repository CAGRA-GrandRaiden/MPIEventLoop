#include "EventLoop.hh"

void EventLoop::Process(const int& entry) {
  m_reader->SetEntry(entry);

  // user defined analysis //////////////////////////////////////

  ATEvent* event = (ATEvent*) eventArray->At(0);
  Int_t nHits = event->GetNumHits();
  std::vector<ATHit>* hitArray = event->GetHitArray();
  event->GetHitArrayObj();
  //std::cout<<event->GetEventID()<<std::endl;
  hitArray->size();

  std::vector<ATHit*>* hitbuff = new std::vector<ATHit*>;


  for(Int_t iHit=0; iHit<nHits; iHit++){
    ATHit hit = event->GetHit(iHit);
    TVector3 hitPos = hit.GetPosition();
    hitbuff->push_back(&hit);
  }

  ATHoughSpaceCircle* fHoughSpaceCircle  = dynamic_cast<ATHoughSpaceCircle*> (houghArray->At(0));
  parameter = fHoughSpaceCircle->GetInitialParameters();
  std::pair<Double_t,Double_t>  fHoughLinePar =  fHoughSpaceCircle->GetHoughPar();
  Double_t HoughAngleDeg = fHoughLinePar.first*180.0/TMath::Pi();
  if (   HoughAngleDeg<90.0 && HoughAngleDeg>45.0 ) {

    min->MinimizeOpt(parameter,event);
  }

}









EventLoop::EventLoop(const char* treename, vector<string> inputs)
  : MPILooper(treename, inputs), eventArray(*m_reader, "ATEventH"), houghArray(*m_reader, "ATHough")
{ Setup(); }

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

  // user defined initialization and setup

  run = new FairRunAna(); //Forcing a dummy run
  min = new MCMinimization();
  parameter = new Double_t[8];
  min->ResetParameters();

}
