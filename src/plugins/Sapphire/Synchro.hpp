#pragma once

#include "Thread/Atomic

namespace Sapphire{

class SynchroCallback {

public:
  void SignalResult();

};

class SynchroProto {
public:
  SynchroProto();
  virtual ~SynchroProto();

  virtual void AwaitResult() = 0;

};

template<typename T>
class Synchro {

public:
  Synchro(T *a);
  virtual ~Synchro();

  virtual void AwaitResult(){

  }



};


}
