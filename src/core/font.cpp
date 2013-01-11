#include "font.h"
#include "filemanager.h"



Font::Font()
{
  this->font.isProportional = false;
  this->font.proportions = NULL;
  this->font.letterSpacing = 0.0f;
}//constructor



Font::Font( const std::string &objectName, Object* parentObject )
:Object( objectName, parentObject )
{
  this->font.isProportional = false;
  this->font.proportions = NULL;
  this->font.letterSpacing = 0.0f;
  this->font.scale.Set( 1.0f, 1.0f );
}//constructor



Font::~Font()
{
  this->ClearText();
  DEF_DELETE( this->font.proportions );
}//destructor




/*
=============
  SetRenderPipeline
=============
*/
void Font::SetRenderPipeline( CoreRenderableList* setRenderableList, CoreRenderableListIndicies* setRenderableIndicies, CoreRenderableListIndicies* setRenderableFreeIndicies )
{
  this->_renderableList         = setRenderableList;
  this->_renderableIndicies     = setRenderableIndicies;
  this->_renderableFreeIndicies = setRenderableFreeIndicies;
}//Font




/*
=============
  SetFont
=============
*/
Font* Font::SetFont( const std::string& fontFileName, const std::string &fileProportions, float setLetterSpacing )
{
  this->font.fontName = fontFileName;
  this->font.letterSpacing = setLetterSpacing;

  this->font.isProportional = ( fileProportions.length() > 0 );
  if( this->font.isProportional )
  {
    memory data;
    if( __fileManager->GetFile( fileProportions, data ) )
    {
      if( !this->font.proportions )
        this->font.proportions = new FontProportions[ 256 ];
      Byte *dataByte = ( Byte* ) data.getData();
      for( Dword num = 0; num < 256; ++num )
      {
        this->font.proportions[ num ].offset = dataByte[ num << 1 ];
        this->font.proportions[ num ].width  = dataByte[ ( num << 1 ) + 1 ];
      }//for num
    }
    else {
      __log.PrintInfo( Filelevel_ERROR, "Font::SetFont => proportions file '%s' not found", fileProportions.c_str() );
      DEF_DELETE( this->font.proportions );
      this->font.isProportional = false;
    }
  }
  else {
    DEF_DELETE( this->font.proportions );
  }

  return this;
}//SetFont



/*
=============
  SetText
=============
*/
void Font::SetText( const std::string setText )
{
  this->ClearText();
  if( !this->_renderableList )
  {
    __log.PrintInfo( Filelevel_ERROR, "Font::SetText => font not initialized, renderable list is NULL" );
    return;
  }

  this->font.text = setText;

  this->_RecalculatePosition();
  float zIndex = this->position.z;

  Renderable *letter = NULL;
  Dword length = setText.length(), q;
  if( !__textureAtlas->IsTextureLoaded( this->font.fontName ) )
    __textureAtlas->LoadTexture( this->font.fontName );
  Vec2 textureSize = __textureAtlas->GetTextureSize( this->font.fontName );
  Vec2 letterToTexCoords( 16.0f / textureSize.x, 16.0f / textureSize.y );
  Vec2 pointToTexCoords( 1.0f / textureSize.x, 1.0f / textureSize.y );  //перевод координат точек в текстуре в текстурные координаты
  Vec2 letterSize( textureSize.x / 16.0f * this->font.scale.x, textureSize.y / 16.0f * this->font.scale.y );
  float dx = 0.0f, letterWidth = letterSize.x;
  Byte letterId;

  for( q = 0; q < length; ++q )
  {
    letterId = setText[ q ];
    GLshort index = -1;
    __log.PrintInfo( Filelevel_DEBUG, "Free indicies in x%X = %d", this->_renderableFreeIndicies, this->_renderableFreeIndicies->size() );
    if( this->_renderableFreeIndicies->size() )
    {
      index = *this->_renderableFreeIndicies->rbegin();
      this->_renderableFreeIndicies->pop_back();
      letter = &( *( this->_renderableList->begin() + index ) );
      *letter = RenderableQuad();
      __log.PrintInfo( Filelevel_DEBUG, "Use free indicies: %d", index );
    }
    else
    {
      this->_renderableList->push_back( RenderableQuad() );
      letter = &( *this->_renderableList->rbegin() );
      index = this->_renderableList->size() - 1;
    }

    RenderableQuad *quad = ( RenderableQuad* ) letter;
    quad->SetPosition( Vec3( this->position.x + dx, this->position.y, zIndex ) );
    if( this->font.isProportional ) {
      quad->SetSize( Vec2(
        textureSize.x / 16.0f * this->font.scale.x * ( this->font.proportions[ letterId ].width / 16.0f ),
        letterSize.y
        ));
      letterWidth = this->font.proportions[ letterId ].width * this->font.scale.x;
    } else {
      quad->SetSize( letterSize );
    }
    quad->SetColor( Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );

    //вычисление текстурных координат
    __log.PrintInfo( Filelevel_DEBUG, "Font::SetText => letter[%c]", letterId );
    Pos< Byte > letterPos( letterId % 16, letterId >> 4 );  //позиция буквы в алфавите
    __log.PrintInfo( Filelevel_DEBUG, ". letterPos[%d; %d]", letterPos.x, letterPos.y );
    Vec2 t0, t1;
    if( this->font.isProportional ) {
      t0.Set( letterPos.x * letterToTexCoords.x + this->font.proportions[ letterId ].offset * pointToTexCoords.x, letterPos.y * letterToTexCoords.y );
      t1.Set( letterPos.x * letterToTexCoords.x + ( this->font.proportions[ letterId ].offset + this->font.proportions[ letterId ].width ) * pointToTexCoords.x, ( 1.0f + letterPos.y ) * letterToTexCoords.y );
    } else {
      t0.Set( letterPos.x * letterToTexCoords.x, letterPos.y * letterToTexCoords.y );
      t1.Set( ( 1.0f + letterPos.x ) * letterToTexCoords.x, ( 1.0f + letterPos.y ) * letterToTexCoords.y );
    }
    quad->SetTexture( this->font.fontName, t0, t1 );

    float z = quad->GetPosition().z;
    this->font.renderableIndicies.push_back( index );

    //поиск и размещение в правильной z-позиции
    CoreRenderableListIndicies::iterator iter, iterEnd = this->_renderableIndicies->end(), iterBegin = this->_renderableIndicies->begin();
    bool added = false;
    for( iter = iterBegin; iter != iterEnd; ++iter )
      if( z < ( *( this->_renderableList->begin() + *iter ) ).GetPosition().z )
      {
        this->_renderableIndicies->insert( iter, index );
        added = true;
        break;
      }
    if( !added )
      this->_renderableIndicies->push_back( index );

    dx += letterWidth + this->font.letterSpacing * this->font.scale.x;
  }//for q
}//SetText



/*
=============
  SetScale
=============
*/
Font* Font::SetScale( const Vec2& setScale )
{
  this->font.scale = setScale;
  return this;
}//SetScale



/*
=============
  ClearText
=============
*/
void Font::ClearText()
{
  if( this->font.text.length() )
  {
    __log.PrintInfo( Filelevel_DEBUG, "Font::ClearText => text['%s'] renderables[%d]", this->font.text.c_str(), this->font.renderableIndicies.size() );
    FontRenderablesList::iterator iter, iterEnd = this->font.renderableIndicies.end();
    for( iter = this->font.renderableIndicies.begin(); iter != iterEnd; ++iter )
    {
      __log.PrintInfo( Filelevel_DEBUG, ". letter renderable index[%d]", *iter );
      CoreRenderableListIndicies::iterator _iter, _iterEnd = this->_renderableIndicies->end();
      for( _iter = this->_renderableIndicies->begin(); _iter != _iterEnd; ++_iter )
      {
        if( *iter == *_iter )
        {
          this->_renderableFreeIndicies->push_back( *_iter );
          this->_renderableIndicies->erase( _iter );
          break;
        }//for iter indicies
      }
    }
    this->font.renderableIndicies.clear();
    this->font.text = "";
  }
}//ClearText
