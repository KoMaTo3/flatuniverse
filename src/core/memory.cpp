/*
--------------------
  �������� ������
--------------------
*/

#include "memory.h"
#include "ktypes.h"
//#include "aFile.h"
//#include "defines.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

memory::memory()
:data(0), length(0) //init()
{
}

memory::memory(DWORD len)
{
  this->init();
  this->Alloc(len);
}//constructor

memory::memory( const memory &src )
{
  this->init();
  this->operator =( (memory&) src);
}//constructor


memory::~memory()
{
  this->Free();
}//destructor


/*
----------
  init
  ��������� ������������� ������
----------
*/
void memory::init()
{
  this->data = 0;
  this->length = 0;
}//init


/*
----------
  operator char*
  ���������� � ���� char*
----------
*/
memory::operator char*()
{
  return this->data;
}//operator char*


/*
----------
  alloc
  ��������� ������. ���� ��� �������� - ������������� � ����� ���������
----------
*/
HRESULT memory::Alloc(DWORD len)
{
  this->Free();

  if(!len)
  {
    //__log.printInfo(aFile_level_WARNING, "memory::alloc(0)");
    this->length = 0;
    return aERR;
  }

  this->length = len;
  this->data = new char[this->length];
  if(!this->data)
  {
    //__log.printInfo(aFile_level_CRITICALERROR, "can't allocate memory, %d byte[s]\n", len);
    return aERR;
  }

  return aOK;
}//alloc


/*
----------
  realloc
  ������������� ������
----------
*/
HRESULT memory::Realloc(DWORD len)
{
  if(this->data)//delete old mem
  {
    this->Free();
  }
  return this->Alloc(len);
}//realloc


/*
----------
  Free
  ������������ ������
----------
*/
void memory::Free()
{
  if(this->data)
  {
    delete[] this->data;
    this->data = 0;
  }
  this->length = 0;
  this->data = 0;
}//Free


/*
----------
  getLength
  ���������� ������ ���������� ������
----------
*/
DWORD memory::getLength() const
{
  return this->length;
}//getLength


/*
----------
  operator = memory
----------
*/
memory& memory::operator=(memory &src)
{
  if(&src == this)
    return *this;
  if(!src.length)
  {
    this->Free();
    return *this;
  }

  if(this->length != src.length)
    this->Alloc(src.length);

  if( this->length ) {
    memcpy(this->data, (char*) src, this->length);
  }
  return *this;
}//operator = memory


/*
----------
  operator = char*
----------
*/
memory& memory::operator=(const char *str)
{
  if(!str)
  {
    this->Free();
    return *this;
  }

  DWORD len = (DWORD) strlen(str) + 1;
  if(this->length != len)
  {
    this->Free();
    this->Alloc(len);
  }
  memcpy(this->data, str, len - 1);
  data[len - 1] = 0;
  return *this;
}//operator = char*


/*
----------
  getData
  ���������� ��������� �� ������ ����� ������
----------
*/
char* memory::getData()
{
  return this->data;
}//getData


/*
----------
  operator []
----------
*/
char& memory::operator[](Int num)
{
  static char tmp = 0;
  if(num < 0 || num >= (Int) this->length)
  {
    //__log.printInfo(aFile_level_ERROR, "memory::operator[](%d) this->length = %d", num, this->length);
    return tmp;
  }
  if(this->data == 0)
  {
    //__log.printInfo(aFile_level_ERROR, "memory::operator[]() this->data = 0", num);
    return tmp;
  }
  return this->data[num];
}//operator []


/*
----------
  operator += memory
----------
*/
memory& memory::operator+=(memory &src)
{
  if(this->length < 1)
    return this->operator=(src);

  memory tmp(this->length);
  memcpy((char*)tmp, this->getData(), this->length);

  this->Alloc(tmp.length + src.getLength());
  memcpy(this->getData(), tmp.getData(), tmp.getLength());
  memcpy(this->getData() + tmp.getLength(), src.getData(), src.getLength());
  return *this;
}//operator += memory

