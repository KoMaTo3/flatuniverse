#include "tags.h"


Tags::Tags() {
}//constructor


Tags::~Tags() {
}//destructor




/*
=============
  IsHasTag
=============
*/
bool Tags::IsHasTag( const std::string& tag ) const {
  TagsList::const_iterator iter, iterEnd = this->tags.end();
  for( iter = this->tags.begin(); iter != iterEnd; ++iter ) {
    if( *iter == tag ) {
      return true;
    }
  }//foreach tags

  return false;
}//HasTag



/*
=============
  AddTag
=============
*/
void Tags::AddTag( const std::string& tag ) {
  if( this->IsHasTag( tag ) ) {
    return;
  }

  this->tags.push_back( tag );
}//AddTag




/*
=============
  RemoveTag
=============
*/
void Tags::RemoveTag( const std::string& tag ) {
  TagsList::const_iterator iter, iterEnd = this->tags.end();
  for( iter = this->tags.begin(); iter != iterEnd; ++iter ) {
    if( *iter == tag ) {
      this->tags.erase( iter );
      return;
    }
  }//foreach tags
}//RemoveTag






/*
=============
  SaveToBuffer
=============
*/
void Tags::SaveToBuffer( MemoryWriter &writer )
{
  Dword count = this->tags.size();
  writer << count;

  TagsList::const_iterator iter, iterEnd = this->tags.end();
  for( iter = this->tags.begin(); iter != iterEnd; ++iter ) {
    writer << *iter;
  }
}//SaveToBuffer



/*
=============
  LoadFromBuffer
=============
*/
void Tags::LoadFromBuffer( MemoryReader &reader )
{
  int i;
  Dword count;
  std::string tag;

  reader >> count;

  this->tags.clear();
  for( i = 0; i < count; ++i ) {
    reader >> tag;
    this->AddTag( tag );
  }//for i
}//LoadFromBuffer
