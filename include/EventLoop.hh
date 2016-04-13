#include "MPILooper.hh"

class CESettings;
class SimEvent;

class EventLoop : public MPILooper {
public:
  EventLoop(vector<string>);
  ~EventLoop();
  virtual void Setup();

private:
  inline virtual void Process(const int& entry);


  CESettings* settings = nullptr;
  SimEvent* simevent = nullptr;

};
