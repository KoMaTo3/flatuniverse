#ifndef __ANIMATIONMANAGER_H__
#define __ANIMATIONMANAGER_H__


#include "animationpack.h"
#include "textparser.h"
#include "interpolation.h"


class AnimationTemplate;


namespace Animation {


class Manager {
public:
  Manager();
  virtual ~Manager();
  void LoadFile( const std::string& fileName );

private:
  void _LoadPack( TextParser &parser );
  void _LoadSet( TextParser &parser, Animation::AnimationPack *pack );
  void _LoadAnimationSprite( TextParser &parser, AnimationSet *set );
  void _Error( TextParser &parser, TextParser::Result &result );
  bool _TextParserNextIsSymbol( TextParser &parser, const std::string& symbol, bool showError = false );
  bool _TextParserNextIsNumber( TextParser &parser, TextParser::Result &value, bool showError = false );
  //void _LoadAttributeTexture( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  //void _LoadAttributeRotation( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  //void _LoadAttributeScale( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  //void _LoadAttributeSize( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  //void _LoadAttributePosition( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeRenderableSize( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeRenderableTextureCoords( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeRenderableTexture( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeRenderableRotation( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributePosition( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeColor( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeEnabled( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );
  void _LoadAttributeCollisionSquare( TextParser &parser, AnimationTemplate *tpl, float time = 0.0f, InterpolationType interpolation = FLAT );

  typedef void (Manager::*LoadAttributeFunc)( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation );
  std::hash_map< std::string, LoadAttributeFunc > _loadFunctionList;
};


}


#endif
