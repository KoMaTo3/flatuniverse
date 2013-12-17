#include "thread.h"
#include "file.h"


Thread::Pipeline::Pipeline()
:currentThreadId( 0 )
{
}

Thread::Pipeline::~Pipeline() {
  if( this->currentThreadId ) {
    //_endthreadex(  );
  }
}


void Thread::Pipeline::Add( void *data, Work::ProcessFunc *processFunc ) {
  Work *work = new Work();
  work->data = data;
  work->status = THREAD_WORK_STATUS_WAIT;
  work->processFunc = processFunc;
  this->workList.push_back( work );
  __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Add => +1" );
}//Add


void Thread::Pipeline::Update() {
  if( this->workList.empty() ) {
    return;
  }
  __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update" );
  WorkList::iterator
    iter = this->workList.begin(),
    iterEnd = this->workList.end(),
    iterNext;
  while( iter != iterEnd ) {
    if( ( *iter )->status == THREAD_WORK_STATUS_DONE || ( *iter )->status == THREAD_WORK_STATUS_ERROR ) {
      __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update => work[%d] finished with code[%d]", this->currentThreadId, ( *iter )->status );
      this->currentThreadId = 0;
      iterNext = iter;
      ++iterNext;
      Work *work = *iter;
      delete work;
      this->workList.erase( iter );
      iter = iterNext;
      continue;
    }
    ++iter;
  }
  
  if( !this->currentThreadId ) {
    __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update => find new work" );
    for( auto &iter: this->workList ) {
      if( iter->status == THREAD_WORK_STATUS_WAIT ) {
        __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update => begin new work..." );
        iter->status = Thread::THREAD_WORK_STATUS_WORK;
        this->currentThreadId = _beginthreadex( NULL, 0, iter->processFunc, &*iter, 0, 0 );
        __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update => runned thread[%d]", this->currentThreadId );
        break;
      }
    }
  }
  __log.PrintInfo( Filelevel_DEBUG, "Thread::Pipeline::Update => done" );
}
