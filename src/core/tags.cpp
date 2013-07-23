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
  __log.PrintInfo( Filelevel_WARNING, "Tags::RemoveTag => tag '%s' not found", tag.c_str() );
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
  Dword dw;
  Dword count;
  std::string tag;

  reader >> count;

  this->tags.clear();
  for( dw = 0; dw < count; ++dw ) {
    reader >> tag;
    this->AddTag( tag );
  }//for i
}//LoadFromBuffer
