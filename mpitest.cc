#include "TUnixSystem.h"
#include "TFile.h"
#include "TChain.h"
#include "TChainElement.h"
#include "TH1F.h"
#include <TROOT.h>
#include <TSystem.h>
#include <TCanvas.h>
#include <TObjArray.h>
#include <TCollection.h>
#include <TChainElement.h>

#include "TRandom3.h"
#include "merger.hh"

#define DIAG
#include "SimEvent.hh"

using namespace std;

int main(int argc, char** argv) {

  MPI_Init(&argc, &argv);
  int size,rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  TChain* ch = new TChain("simtree");
  ch->Add("/mnt/analysis/rcnp-e441/sims/segcorrect/output??.root");


  int nentries = ch->GetEntries();
  int threadcount = nentries/size;
  int remainder = nentries - threadcount*size;
  int lowerbound = 0;
  if (rank < remainder) {
    threadcount++;
    lowerbound = rank*threadcount;
  } else {
    //remainder*(threadcount+1)+(size-remainder)*threadcount = nentries
    lowerbound = remainder*(threadcount+1)+(rank-remainder)*threadcount;
  }  
  cout << "Rank: "<<rank<< " EventRange: ["<<lowerbound<< ","<<lowerbound+threadcount<<"]"<<endl;  

  string path = "./tmp/";
  string tmpfile = "0_"+to_string(rank);
  stringstream string;
  string.str(""); string << path << tmpfile << ".root";

  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0) {gSystem->MakeDirectory(path.c_str());}
  



  // Analysis /////////////////////////////////////////////////////////////////////////


  TObjArray *fileElements=ch->GetListOfFiles();
  TIter next(fileElements);
  TChainElement *chEl=0;
  int FileSuccessCount=0;
  CESettings* settings = NULL;
  
  int count=0;
  while (( chEl=(TChainElement*)next() )) {
    TFile f(chEl->GetTitle());
    
    if (f.IsOpen()){
      FileSuccessCount++;
    }else {
      cout<<"Warning could not open "<<chEl->GetTitle()<<endl;
    }
    if (f.GetListOfKeys()->Contains("CESettings") && settings == NULL) {
      stringstream ss;
      ss << "settings";
      TString newName = ss.str().c_str();
      settings = (CESettings*)f.Get("CESettings");
      //      settings->SetName(newName);

    }

    count++;
  }

  if (settings == NULL) {
    cout << "No CESettings object found. Exiting." << endl;
    return 1;
  }

  if ( FileSuccessCount == 0 ){
    cout<<"No files were successfully opened"<<endl;
    return 1;
  }

  // dynamically build simevent structure to contain the right number of clovers
  SimEvent *simevent = new SimEvent(settings);
  ch->SetBranchAddress("simevent",&simevent);


  for (int i = 0; i < settings->CAGRASettings.nClovers; i++) {
    simevent->CAGRA.AddClover();
  }
  settings->CAGRASettings.Reset();
  
  
  TFile output(string.str().c_str(),"recreate");
  TH1F* h1 = new TH1F("CryDoppler00","CryDoppler00",5500,0,4.2);
  int testcount = 0;

  for (int i=lowerbound; i<lowerbound+threadcount; i++) {
    ch->GetEntry(i);
    double beta = simevent->DecayProductsLab.Parent.GetBeta();    
    double energy = simevent->CAGRA.GetClover(0)->GetCrystal(0)->GetEnergy();
    if (energy > 0) {
      energy = gRandom->Gaus(energy, 0.003*energy / 2.355); // <------------ RESOLUTION
      double dop = simevent->CAGRA.GetClover(0)->GetCrystal(0)->DopplerCorrect(energy,beta);
      h1->Fill(dop);
    }
    //if (++testcount==100000) {break;}
    if (rank == 0 && i%50000==0) cout << ((float)i)/(lowerbound+threadcount)*100 << " %" << endl;
  }
  cout << "Rank "<<rank <<": Finished" << endl;
  output.Write();
  output.Close();
  ////////////////////////////////////////////////////////////////////////////////////

  
  
  MPI_Barrier(MPI_COMM_WORLD);

  mt_binarytree_merge("output.root",path.c_str(),size/2,rank,size);
  //mt_binarytree_merge(path.c_str(),1,rank,size);
  

  
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank==0){string.str(""); string << "rm -rf " << path; system(string.str().c_str());}
  //MPI_Send(memhist,sizeof(TH1F),MPI_UNSIGNED_CHAR,1,0,MPI_COMM_WORLD);
  //MPI_Recv(mem,sizeof(TH1F),MPI_UNSIGNED_CHAR,0,0,MPI_COMM_WORLD,&status);
  MPI_Finalize();
  
  return 0;

}
