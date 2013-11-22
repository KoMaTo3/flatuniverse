#ifndef __memory_h__
#define __memory_h__

//### includes ###//

#include "windows.h"
//#include "types.h"
//#include "tools.h"


class memory
{
private:
  DWORD length;
  char *data;
  void init();

public:

  memory();
  memory(DWORD len);
  memory( const memory &src );
  virtual ~memory();

  HRESULT alloc     (DWORD len);
  HRESULT realloc   (DWORD len);
  void   free      ();
  DWORD  getLength () const;
  char*  getData   ();

  memory& operator=   (const char *str);
  memory& operator=   (memory &src);
  char& operator[] (int num);
  memory& operator+=  (memory &src);
  operator char*   ();
};


#endif