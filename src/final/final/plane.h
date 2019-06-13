#ifndef PLANE_H
#define PLANE_H

#include <string>
#include <vector>

#include <stb_image.h>

#include "object.h"

class Plane : public Object
{
  struct Vertex
  {
    glm::vec3 Position;
    glm::vec2 TextCoords;
  };

public:
  Plane(glm::vec3 pos, glm::vec3 size, glm::vec3 color, const char *file) : Object(pos, size, color)
  {
    this->InitRenderData(file);
  };
  ~Plane(){};

  unsigned int VAO;
  int size;

  void Draw(Shader *shader)
  {
    shader->use();
    glm::mat4 model(1.0f);
    model = glm::translate(model, this->Position);
    model = glm::scale(model, this->Size);
    shader->setMat4("model", model);
    glBindVertexArray(this->VAO);
    glDrawArrays(GL_TRIANGLES, 0, this->size);
    glBindVertexArray(0);
  }

  void InitRenderData(const char *file)
  {
    int width, height;
    unsigned char *image = stbi_load(file, &width, &height, 0, STBI_grey);
    // Now generate texture
    std::vector<Vertex> vertices;

    for (int i = 0; i < height - 1; i++)
    {
      for (int k = 0; k < width - 1; k++)
      {
        glm::vec3 v1(k + 0, static_cast<float>(image[k + i * height]), i + 0);
        glm::vec3 v2(k + 0, static_cast<float>(image[k + (i + 1) * height]), i + 1);
        glm::vec3 v3(k + 1, static_cast<float>(image[(k + 1)  + (i + 1) * height]), i + 1);
        glm::vec3 v4(k + 1, static_cast<float>(image[(k + 1) + i * height]), i + 0);
        Vertex t1, t2, t3, t4;
        t1.Position = v1;
        t2.Position = v2;
        t3.Position = v3;
        t4.Position = v4;
        vertices.push_back(t1);
        vertices.push_back(t2);
        vertices.push_back(t3);
        vertices.push_back(t1);
        vertices.push_back(t3);
        vertices.push_back(t4);
      }
    }

    this->size = vertices.size();
    for (int i = 0; i < vertices.size(); i++)
    {
      glm::vec2 tmp(vertices[i].Position.x / width, vertices[i].Position.z / height);
      vertices[i].TextCoords = tmp;
    }

    stbi_image_free(image);

    unsigned int VBO;
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
	glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, TextCoords));
  }
};

#endif