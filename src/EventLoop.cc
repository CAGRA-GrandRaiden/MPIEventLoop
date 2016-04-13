#include "EventLoop.hh"
#define DIAG
#include "SimEvent.hh"

void EventLoop::Process(const int& entry) {
  m_chain->GetEntry(entry);

  // user defined analysis
  double beta = simevent->DecayProductsLab.Parent.GetBeta();    
  double energy = simevent->CAGRA.GetClover(0)->GetCrystal(0)->GetEnergy();
  if (energy > 0) {
    energy = gRandom->Gaus(energy, 0.003*energy / 2.355); // <------------ RESOLUTION
    double dop = simevent->CAGRA.GetClover(0)->GetCrystal(0)->DopplerCorrect(energy,beta);
    Hist("CryDoppler00",dop,5500,0,4.2);
   }






}

EventLoop::EventLoop(vector<string> inputs)
  : MPILooper(inputs) { Setup(); }

EventLoop::~EventLoop(){

  // user defined cleanup (delete EventLoop heap members)
  delete settings;
  delete simevent;
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
    if (f.GetListOfKeys()->Contains("CESettings") && settings == nullptr) {
      stringstream ss;
      ss << "settings";
      TString newName = ss.str().c_str();
      settings = (CESettings*)f.Get("CESettings");
      // settings->SetName(newName);
    }


  }

  if (settings == nullptr) {
    cout << "No CESettings object found. Exiting." << endl;
    exit(0);
  }


  if ( FileSuccessCount == 0 ){
    cout<<"No files were successfully opened"<<endl;
    exit(0);
  }

  // associate userdefined event object with TChain, e.g.
  // dynamically build simevent structure to contain the right number of clovers
  simevent = new SimEvent(settings);
  m_chain->SetBranchAddress("simevent",&simevent);


  for (int i = 0; i < settings->CAGRASettings.nClovers; i++) {
    simevent->CAGRA.AddClover();
  }
  settings->CAGRASettings.Reset();


}
