#ifndef OBJECT_H
#define OBJECT_H

#include <glm/glm.hpp>
#include "resource_manager.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Object
{
public:
  glm::vec3 Position, Size;
  glm::vec3 Color;
  Object::Object(glm::vec3 pos, glm::vec3 size, glm::vec3 color)
    : Position(pos), Size(size), Color(color) {}
  // Draw sprite
  virtual void Draw();
  virtual void InitRenderData();
};

#endif