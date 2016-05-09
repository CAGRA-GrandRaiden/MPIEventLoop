#include "MPILooper.hh"

#include "MCSrc.hh"
#include "MCMinimization.hh"
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <map>
#include <vector>

#include "TClonesArray.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TTreePlayer.h"
#include "TTreeReaderValue.h"
#include "TSystem.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TStopwatch.h"

#include "ATEvent.hh"
#include "ATPad.hh"
#include "ATHit.hh"
#include "ATHoughSpace.hh"
#include "ATHoughSpaceLine.hh"
#include "ATHoughSpaceCircle.hh"

#include "FairRootManager.h"
#include "FairLogger.h"
#include "FairRun.h"
#include "FairRunAna.h"


class EventLoop : public MPILooper {
public:
  EventLoop(const char* treename, vector<string>);
  ~EventLoop();
  virtual void Setup();

private:
  inline virtual void Process(const int& entry);

  // user defined member variables
  FairRunAna* run;
  MCMinimization* min;
  Double_t* parameter;

  TTreeReaderValue<TClonesArray> eventArray;
  TTreeReaderValue<TClonesArray> houghArray;



};
