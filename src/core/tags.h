#pragma once

#include "tools.h"
#include "memorywriter.h"
#include "memoryreader.h"
#include <deque>
#include <string>


class ITags {
public:
  ITags(){}
  virtual ~ITags(){}
  virtual bool IsHasTag ( const std::string& tag ) const = 0;
  virtual void AddTag   ( const std::string& tag ) = 0;
  virtual void RemoveTag( const std::string& tag ) = 0;
};





class Tags: public ITags {
public:
    Tags();
  ~Tags();
  bool IsHasTag       ( const std::string& tag ) const;
  void AddTag         ( const std::string& tag );
  void RemoveTag      ( const std::string& tag );
  void SaveToBuffer   ( MemoryWriter &writer );
  void LoadFromBuffer ( MemoryReader &reader );

private:
  typedef std::deque< std::string > TagsList;

  TagsList tags;

  DISALLOW_COPY_AND_ASSIGN( Tags );
};
