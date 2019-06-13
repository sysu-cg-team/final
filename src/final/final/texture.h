/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#ifndef TEXTURE_H
#define TEXTURE_H
#include <glad/glad.h>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
  // Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
  unsigned int ID;
  // Texture image dimensions
  int Width, Height; // Width and height of loaded image in pixels
  // Texture Format
  int Internal_Format; // Format of texture object
  int Image_Format;    // Format of loaded image
  // Texture configuration
  int Wrap_S;     // Wrapping mode on S axis
  int Wrap_T;     // Wrapping mode on T axis
  int Filter_Min; // Filtering mode if texture pixels < screen pixels
  int Filter_Max; // Filtering mode if texture pixels > screen pixels
  // Constructor (sets default texture modes)
  Texture2D();
  // Generates texture from image data
  void Generate(int width, int height, unsigned char *data);
  // Binds the texture as the current active GL_TEXTURE_2D texture object
  void Bind() const;
};

#endif