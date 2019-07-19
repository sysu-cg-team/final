#ifndef SKYBOX_H
#define SKYBOX_H

#include <string>
#include <vector>

#include <stb_image.h>

#include "object.h"


class Skybox : public Object
{
public:
	Skybox(glm::vec3 pos, glm::vec3 size, glm::vec3 color, std::vector<std::string> faces, std::vector<std::string> faces2, std::string name, std::string name2) : Object(pos, size, color) {
    this->faces = faces;
    this->name = name;
	this->faces2 = faces2;
	this->name2 = name2;
    this->InitRenderData();
	};
	~Skybox() {};

	unsigned int VAO;

	void Draw(Shader *shader) {
    glDepthFunc(GL_LEQUAL);
		shader->use();
		shader->setInt("skybox1", 0);
		shader->setInt("skybox2", 1);
		glm::mat4 model(1.0f);
		model = glm::translate(model, this->Position);
		model = glm::scale(model, this->Size);
		shader->setMat4("model", model);

		auto texture = ResourceManager::GetTexture(this->name);
		auto night = ResourceManager::GetTexture(this->name2);
		glBindVertexArray(this->VAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture.ID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_CUBE_MAP, night.ID);
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
    auto cubemapTexture2 = this->loadCubemap(this->faces2);
	Texture2D t;
	t.ID = cubemapTexture;
    ResourceManager::Textures[this->name] = t;
	Texture2D t2;
	t2.ID = cubemapTexture2;
    ResourceManager::Textures[this->name2] = t2;
  }

  private:
    std::vector<std::string> faces;
    std::string name;
	std::vector<std::string> faces2;
	std::string name2;
    unsigned int loadCubemap(std::vector<std::string> faces);
};

#endif