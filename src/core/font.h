#pragma once

#include "object.h"



class Font: public Object
{
private:
  struct FontProportions {
    float offset, //смещение начала буквы от левого края
          width;  //ширина буквы
  };

private:
  typedef std::vector< GLushort > FontRenderablesList;
  struct
  {
    std::string     text;     //текст
    std::string     fontName; //имя шрифта (файла со шрифтом)
    Vec2            scale;    //масштабы шрифта
    float           letterSpacing;  //расстояние между буквами

    bool            isProportional;  //шрифт пропорциональный
    FontProportions *proportions; //сами пропорции

    FontRenderablesList renderableIndicies;
  } font;

public:
  Font();
  Font( const std::string &objectName, Object* parentObject );
  virtual ~Font();

  void  SetRenderPipeline ( CoreRenderableList* setRenderableList, CoreRenderableListIndicies* setRenderableIndicies, CoreRenderableListIndicies* setRenderableFreeIndicies );
  Font* SetFont           ( const std::string& fontFileName, const std::string &fileProportions = "", float setLetterSpacing = 0.0f );
  void  SetText           ( const std::string& setText );
  void  ClearText         ();
  Font* SetScale          ( const Vec2& setScale );
  float GetTextWidth      ( const std::string &text );
};
