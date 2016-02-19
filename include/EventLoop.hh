#include "MPILooper.hh"

class CESettings;
class SimEvent;

class EventLoop : public MPILooper {
public:
  EventLoop(vector<string>);
  ~EventLoop();
  virtual void Setup();

private:
  virtual void Process(const int& entry);  
  virtual void Run();

  CESettings* settings = nullptr;
  SimEvent* simevent = nullptr;
  TH1F* h1;  
};
