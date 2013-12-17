/*==============================
  Класс ImageLoader
  Загрузка изображений из файлов.
  Изображения формируются в памяти в формате RGBA.

  Класс ImgJpg
  Загрузка jpeg используя стороннюю библиотеку.

  Класс ImgPng
  Загрузка png используя стороннюю библиотеку.
==============================*/

#pragma once

#include "klib.h"
#include "memory.h"
#include "string"
#include "memoryreader.h"

#define XMD_H
extern "C" {
#include "jpeglib/jpeglib.h"
}
#include "setjmp.h"
#include "pnglib/png.h"


using namespace std;
enum IMAGELOADER_IMAGE_TYPE {
  IMAGELOADER_IMAGE_TYPE_UNKNOWN,
  IMAGELOADER_IMAGE_TYPE_TGA,
  IMAGELOADER_IMAGE_TYPE_BMP,
  IMAGELOADER_IMAGE_TYPE_PNG,
  IMAGELOADER_IMAGE_TYPE_JPG,
};


class ImageLoader
{
private:
  memory      _data;
  Size        _size;  //ширина, высота

private:
  Bool  _LoadFromBufferBMP ( Byte *buffer, Dword length );
  Bool  _LoadFromBufferJPG ( Byte *buffer, Dword length );
  Bool  _LoadFromBufferPNG ( Byte *buffer, Dword length );
  Bool  _LoadFromBufferTGA ( Byte *buffer, Dword length );

public:
  ImageLoader();
  virtual ~ImageLoader();

  Bool        LoadFromFile  ( const string &fileName );
  Bool        LoadFromBuffer( Byte *buffer, Dword length );
  const Size  GetImageSize  ();
  inline
  Byte*       GetImageData  () { return (Byte*) this->_data.getData(); }

  static IMAGELOADER_IMAGE_TYPE GetImageFormat( Byte *buffer, Dword length );
  static const Size GetImageSizeFromBuffer( Byte *buffer, Dword length );
  static const Size GetImageSizeBMP ( Byte *buffer, Dword length );
  static const Size GetImageSizeTGA ( Byte *buffer, Dword length );
  static const Size GetImageSizeJPG ( Byte *buffer, Dword length );
  static const Size GetImageSizePNG ( Byte *buffer, Dword length );
};


/*==============================
  ImgJpg
==============================*/

class ImgJpg
{
public:

  struct  _gImporterJPEGError {
          jpeg_error_mgr  pub;
          jmp_buf setjmp_buffer;
  };

  static const int c_JPEGInputBufferSize;

  static void JPEGErrorExit             ( j_common_ptr cinfo );
  static void JPEGStdioSrcgMemoryReader ( j_decompress_ptr cinfo, MemoryReader* file );
};


/*==============================
  ImgPng
==============================*/

class ImgPng
{
public:
  static Void PNGReadFunctiongFile        ( png_structp png_ptr, png_bytep data, png_size_t length );
  static Void PNGReadFunctiongMemoryReader( png_structp png_ptr, png_bytep data, png_size_t length );

};
