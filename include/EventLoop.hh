#include "MPILooper.hh"

class EventLoop : public MPILooper {
public:
  EventLoop(const char* treename, vector<string>);
  ~EventLoop();
  virtual void Setup();

private:
  inline virtual void Process(const int& entry);

  // user defined member variables

};
