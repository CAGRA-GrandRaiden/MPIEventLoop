#include "MPILooper.hh"

class EventLoop : public MPILooper {
public:
  EventLoop(vector<string>);
  ~EventLoop();
  virtual void Setup();

private:
  inline virtual void Process(const int& entry);

  // user defined member variables

};
