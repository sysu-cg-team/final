#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>

#include <stb_image.h>

#include "object.h"


class Skybox : public Object
{
public:
	Skybox(glm::vec3 pos, glm::vec3 size, glm::vec3 color, std::vector<std::string> faces, std::string name) : Object(pos, size, color) {
    this->faces = faces;
    this->name = name;
    this->InitRenderData();
	};
	~Skybox() {};

	unsigned int VAO;

	void Draw(Shader *shader) {
    glDepthFunc(GL_LEQUAL);
		shader->use();
		shader->setInt("skybox", 0);
		glm::mat4 model(1.0f);
		model = glm::translate(model, this->Position);
		model = glm::scale(model, this->Size);
		shader->setMat4("model", model);

		auto texture = ResourceManager::GetTexture(this->name);
    glBindVertexArray(this->VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture.ID);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
	}

	void InitRenderData() {
    float vertices[] = {
        -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,

        -1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f};

    unsigned int VBO;
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    // load cubeTextures to Resources
    auto cubemapTexture = this->loadCubemap(this->faces);
	Texture2D t;
	t.ID = cubemapTexture;
    ResourceManager::Textures[this->name] = t;
  }

  private:
    std::vector<std::string> faces;
    std::string name;
    unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif