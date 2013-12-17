#ifndef __THREAD_H__
#define __THREAD_H__

#include <process.h>
#include "deque"


namespace Thread {

enum THREAD_WORK_STATUS {
  THREAD_WORK_STATUS_UNKNOWN,
  THREAD_WORK_STATUS_WAIT,
  THREAD_WORK_STATUS_WORK,
  THREAD_WORK_STATUS_DONE,
  THREAD_WORK_STATUS_ERROR,
};

struct Work {
  typedef unsigned int __stdcall ProcessFunc( void* );
  volatile THREAD_WORK_STATUS status;
  void *data;
  ProcessFunc *processFunc;
};

class Pipeline {
public:
  typedef std::deque< Work* > WorkList;

  Pipeline();
  virtual ~Pipeline();
  void Add( void *data, Work::ProcessFunc *processFunc );
  void Update();
  void Init();

private:
  WorkList workList;
  uintptr_t currentThreadId;
};

};



#endif
