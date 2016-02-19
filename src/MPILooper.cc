#include "MPILooper.hh"
#include "merger.hh"

MPILooper::MPILooper(vector<string> inputlist) {

  MPI_Comm_size(MPI_COMM_WORLD, &m_size);
  MPI_Comm_rank(MPI_COMM_WORLD, &m_rank);
  m_chain = make_shared<TChain>("simtree");

  for(auto fn : inputlist) {
    m_chain->Add(fn.c_str());
  }

  
  m_nentries = m_chain->GetEntries();
  m_threadcount = m_nentries/m_size;
  m_lowerbound = 0;
  int remainder = m_nentries - m_threadcount*m_size;

  if (m_rank < remainder) {
    m_threadcount++;
    m_lowerbound = m_rank*m_threadcount;
  } else {
    m_lowerbound = remainder*(m_threadcount+1)+(m_rank-remainder)*m_threadcount;
  }
  m_upperbound = m_lowerbound+m_threadcount;
  cout << "Rank: "<<m_rank<< " EventRange: ["<<m_lowerbound<< ","<<m_upperbound<<"]"<<endl;  

  m_tmpfile = "0_"+to_string(m_rank);
  m_string.str(""); m_string << m_path << m_tmpfile << ".root";
 
  MPI_Barrier(MPI_COMM_WORLD);
  if (m_rank==0) {gSystem->MakeDirectory(m_path.c_str());}
  m_output = make_shared<TFile>(m_string.str().c_str(),"recreate");

}
void MPILooper::Finalize(){
  m_output->Write();
  m_output->Close();
  
  MPI_Barrier(MPI_COMM_WORLD);
  mt_binarytree_merge(m_outputpath.c_str(),m_path.c_str(),m_size/2,m_rank,m_size);

}

void MPILooper::Run() {
  for (int i=m_lowerbound; i<m_upperbound; i++) {
    Process(i);
  }
}

MPILooper::~MPILooper() {

  this->Finalize();
  MPI_Barrier(MPI_COMM_WORLD);
  if (m_rank==0){m_string.str(""); m_string << "rm -rf " << m_path; system(m_string.str().c_str());}

  MPI_Finalize();  
}
