#include "animationmanager.h"
#include "animationtemplate.h"
#include "file.h"
#include "memory.h"
#include "textparser.h"
#include "file.h"


using namespace Animation;


Manager::Manager() {
  //this->_loadFunctionList.insert( std::make_pair( "scale", &Manager::_LoadAttributeScale ) );
  //this->_loadFunctionList.insert( std::make_pair( "rotation", &Manager::_LoadAttributeRotation ) );
  this->_loadFunctionList.insert( std::make_pair( "renderable_size", &Manager::_LoadAttributeRenderableSize ) );
  this->_loadFunctionList.insert( std::make_pair( "renderable_texture_coords", &Manager::_LoadAttributeRenderableTextureCoords ) );
  this->_loadFunctionList.insert( std::make_pair( "renderable_texture", &Manager::_LoadAttributeRenderableTexture ) );
  this->_loadFunctionList.insert( std::make_pair( "renderable_rotation", &Manager::_LoadAttributeRenderableRotation ) );
  this->_loadFunctionList.insert( std::make_pair( "position", &Manager::_LoadAttributePosition ) );
  this->_loadFunctionList.insert( std::make_pair( "color", &Manager::_LoadAttributeColor ) );
  //this->_loadFunctionList.insert( std::make_pair( "position", &Manager::_LoadAttributePosition ) );
}


Manager::~Manager() {
}


void Manager::LoadFile( const std::string& fileName ) {
  File f;
  if( f.Open( fileName, File_mode_READ ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::LoadFile => can't open file" );
    return;
  }
  memory data( f.GetLength() + 1 );
  f.Read( data.getData(), f.GetLength() );
  f.Close();
  data[ data.getLength() - 1 ] = 0;

  TextParser parser( data.getData(), data.getLength() );
  TextParser::Result value;
  while( parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "template" ) {
        this->_LoadPack( parser );
      }//template
    break;
    }//switch type
  }//while
}//LoadFile


void Manager::_LoadPack( TextParser &parser ) {
  TextParser::Result value;

  //"name"
  if( !parser.GetNext( value ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::_LoadPack => end of file" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  Animation::AnimationPack *pack = new Animation::AnimationPack( value.value, true );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    return;
  }

  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "animation" ) {
        this->_LoadSet( parser, pack );
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

}//_LoadPack


void Manager::_Error( TextParser &parser, TextParser::Result &result ) {
  __log.PrintInfo( Filelevel_ERROR, "Animation::Manager => error at line %d column %d", parser.GetLineAtByte( result.lastPointer ), parser.GetColumnAtByte( result.lastPointer ) );
}


void Manager::_LoadSet( TextParser &parser, Animation::AnimationPack *pack ) {
  TextParser::Result value;

  //"name"
  if( !parser.GetNext( value ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::_LoadSet => end of file" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  const std::string name = value.value;

  //length
  if( !this->_TextParserNextIsNumber( parser, value ) ) {
    return;
  }
  float length = value.GetFloat();

  AnimationSet *set = pack->CreateAnimationSet( name, length );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    return;
  }

  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "sprite" ) {
        this->_LoadAnimationSprite( parser, set );
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

}//_LoadSet


void Manager::_LoadAnimationSprite( TextParser &parser, AnimationSet *set ) {
  TextParser::Result value;

  //name
  if( !parser.GetNext( value ) ) {
    __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::_LoadAnimationSprite => end of file" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  const std::string name = value.value;
  AnimationTemplate *tpl = new AnimationTemplate( name );
  set->AddAnimation( tpl );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    this->_Error( parser, value );
    return;
  }
  
  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING: {
      auto func = this->_loadFunctionList.find( value.value );
      if( func != this->_loadFunctionList.end() ) {
        if( !parser.GetNext( value ) || value.type != TPL_SYMBOL ) {
          this->_Error( parser, value );
          return;
        }
        if( value.value == "(" ) { //one value, without timeline
          parser.PopLastValue( value );
          (this->*func->second)( parser, tpl, 0.0f, FLAT );
        } else if( value.value == "{" ) { //multiple values by time
          bool isDoneBrackets = false;
          bool timeSetted = false;
          bool interpolationSetted = false;
          float
            defaultTime = 0.0f,
            time = defaultTime;
          InterpolationType
            defaultInterpolation = FLAT,
            interpolation = defaultInterpolation;
          while( !isDoneBrackets && parser.GetNext( value ) ) {
            switch( value.type ) {
            case TPL_NUMBER:
              if( !timeSetted ) {
                time = value.GetFloat();
                timeSetted = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            case TPL_STRING:
              if( !interpolationSetted ) {
                interpolation = Interpolation::StringToInterpolation( value.value );
                interpolationSetted = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            case TPL_SYMBOL:
              if( value.value == "(" ) {
                parser.PopLastValue( value );
                (this->*func->second)( parser, tpl, time, interpolation );
                time = defaultTime;
                interpolation = defaultInterpolation;
                timeSetted = false;
                interpolationSetted = false;
              } else if( value.value == "}" ) {
                isDoneBrackets = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            }//switch
          }//while
        } else {
          this->_Error( parser, value );
          return;
        }
      } else {
        this->_Error( parser, value );
        return;
      }
      /*
      if( value.value == "texture" ) {
        this->_LoadAttributeTexture( parser, tpl );
      } else if( value.value == "rotation" ) {
        this->_LoadAttributeRotation( parser, tpl );
      } else if( value.value == "scale" ) {
        this->_LoadAttributeScale( parser, tpl );
      } else {
        this->_Error( parser, value );
        return;
      }
      */
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }
  }//while

}//_LoadAnimationSprite


bool Manager::_TextParserNextIsSymbol( TextParser &parser, const std::string& symbol, bool showError ) {
  TextParser::Result value;
  if( !parser.GetNext( value ) ) {
    if( showError ) {
      __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::_TextParserNextIsSymbol => end of file" );
    }
    return false;
  }
  if( value.value != symbol ) {
    if( showError ) {
      this->_Error( parser, value );
    }
    return false;
  }
  return true;
}//_TextParserNextIsSymbol


bool Manager::_TextParserNextIsNumber( TextParser &parser, TextParser::Result &value, bool showError ) {
  if( !parser.GetNext( value ) ) {
    if( showError ) {
      __log.PrintInfo( Filelevel_ERROR, "Animation::Manager::_TextParserNextIsNumber => end of file" );
    }
    return false;
  }
  if( value.type != TPL_NUMBER ) {
    if( showError ) {
      this->_Error( parser, value );
    }
    return false;
  }
  return true;
}//_TextParserNextIsNumber


void Manager::_LoadAttributeRenderableTexture( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  bool textureNameSetted = false;
  bool textureCoordsSetted = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_STRING:  //texture file name
      if( !textureNameSetted ) {
        __log.PrintInfo( Filelevel_DEBUG, "_LoadAttributeRenderableTexture => name[%s]", value.value.c_str() );
        static_cast< AnimationParameterString* >( tpl->SetParameter< AnimationParameterString >( RENDERABLE_TEXTURE_NAME ) )->AddKeyFrame( time, value.value );
        textureNameSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( !textureCoordsSetted ) {
        Vec4 uv;
        uv.x = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.y = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.z = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.w = value.GetFloat();
        static_cast< AnimationParameterFloat4* >( tpl->SetParameter< AnimationParameterFloat4 >( RENDERABLE_TEXTURE_COORDINATES ) )->AddKeyFrame( time, uv, interpolation );
        textureCoordsSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while
  
}//_LoadAttributeRenderableTexture


/*
void Manager::_LoadAttributeRotation( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  bool angleSetted = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( !angleSetted ) {
        float angle = value.GetFloat();
        static_cast< AnimationParameterFloat1* >( tpl->SetParameter< AnimationParameterFloat1 >( ROTATION ) )->AddKeyFrame( time, angle, interpolation );
        angleSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while
  
}//_LoadAttributeRotation


void Manager::_LoadAttributeScale( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int scaleSettingStep = 0;
  Vec2 scale( Vec2One );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( scaleSettingStep < 2 ) {
        switch( scaleSettingStep ) {
        case 0:
          scale.x = scale.y = value.GetFloat();
        break;
        case 1:
          scale.y = value.GetFloat();
        break;
        }
        ++scaleSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( scaleSettingStep > 0 ) {
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( SCALE ) )->AddKeyFrame( time, scale, interpolation );
  }

}//_LoadAttributeScale
*/


void Manager::_LoadAttributeRenderableSize( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int sizeSettingStep = 0;
  Vec2 size( Vec2Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( sizeSettingStep < 2 ) {
        switch( sizeSettingStep ) {
        case 0:
          size.x = size.y = value.GetFloat();
        break;
        case 1:
          size.y = value.GetFloat();
        break;
        }
        ++sizeSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( sizeSettingStep > 0 ) {
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( RENDERABLE_SIZE ) )->AddKeyFrame( time, size, interpolation );
  }

}//_LoadAttributeRenderableSize


void Manager::_LoadAttributePosition( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int positionSettingStep = 0;
  Vec2 position( Vec2Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( positionSettingStep < 2 ) {
        switch( positionSettingStep ) {
        case 0:
          position.x = position.y = value.GetFloat();
        break;
        case 1:
          position.y = value.GetFloat();
        break;
        }
        ++positionSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( positionSettingStep > 0 ) {
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( POSITION ) )->AddKeyFrame( time, position, interpolation );
  }

}//_LoadAttributePosition


void Manager::_LoadAttributeRenderableRotation( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  float rotation = 0.0f;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      rotation = value.GetFloat();
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  static_cast< AnimationParameterFloat1* >( tpl->SetParameter< AnimationParameterFloat1 >( RENDERABLE_ROTATION ) )->AddKeyFrame( time, rotation, interpolation );

}//_LoadAttributeRenderableRotation


void Manager::_LoadAttributeRenderableTextureCoords( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int coordsSettingStep = 0;
  Vec4 coords( Vec4Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( coordsSettingStep < 4 ) {
        switch( coordsSettingStep ) {
        case 0:
          coords.x = coords.y = coords.z = coords.w = value.GetFloat();
        break;
        case 1:
          coords.z = coords.w = value.GetFloat();
        break;
        case 2:
          coords.y = coords.w;
          coords.z = value.GetFloat();
        break;
        case 3:
          coords.w = value.GetFloat();
        break;
        }
        ++coordsSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( coordsSettingStep > 0 ) {
    __log.PrintInfo( Filelevel_DEBUG, "RENDERABLE_TEXTURE_COORDINATES => [%3.3f; %3.3f]-[%3.3f; %3.3f] step[%d]", coords.x, coords.y, coords.z, coords.w, coordsSettingStep );
    static_cast< AnimationParameterFloat4* >( tpl->SetParameter< AnimationParameterFloat4 >( RENDERABLE_TEXTURE_COORDINATES ) )->AddKeyFrame( time, coords, interpolation );
  }

}//_LoadAttributeRenderableTextureCoords


void Manager::_LoadAttributeColor( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int colorSettingStep = 0;
  Vec4 color( Vec4Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( colorSettingStep < 4 ) {
        switch( colorSettingStep ) {
        case 0:
          color.x = color.y = color.z = color.w = value.GetFloat();
        break;
        case 1:
          color.y = value.GetFloat();
        break;
        case 2:
          color.z = value.GetFloat();
        break;
        case 3:
          color.w = value.GetFloat();
        break;
        }
        ++colorSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( colorSettingStep > 0 ) {
    __log.PrintInfo( Filelevel_DEBUG, "COLOR => [%3.3f; %3.3f]-[%3.3f; %3.3f] step[%d]", color.x, color.y, color.z, color.w, colorSettingStep );
    static_cast< AnimationParameterFloat4* >( tpl->SetParameter< AnimationParameterFloat4 >( COLOR ) )->AddKeyFrame( time, color, interpolation );
  }

}//_LoadAttributeColor


/*
void Manager::_LoadAttributePosition( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int positionSettingStep = 0;
  Vec2 position( Vec2Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( positionSettingStep < 2 ) {
        switch( positionSettingStep ) {
        case 0:
          position.x = position.y = value.GetFloat();
        break;
        case 1:
          position.y = value.GetFloat();
        break;
        }
        ++positionSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( positionSettingStep > 0 ) {
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( POSITION ) )->AddKeyFrame( time, position, interpolation );
  }

}//_LoadAttributePosition
*/
