#ifndef WOOD_H
#define WOOD_H

#include "object.h"

class Wood : public Object
{
public:
	Wood(glm::vec3 pos, glm::vec3 size, glm::vec3 color) : Object(pos, size, color) {
		this->InitRenderData();
	};
	~Wood() {};

	unsigned int VAO;

	void Draw(Shader *shader) {
		shader->use();
		shader->setInt("texture1", 0);
		glm::mat4 model(1.0f);
		model = glm::translate(model, this->Position);
		model = glm::scale(model, this->Size);
		shader->setMat4("model", model);
		auto texture = ResourceManager::GetTexture("wood");
		glActiveTexture(GL_TEXTURE0);
		texture.Bind();
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	void InitRenderData() {
		float vertices[] = {
		  -2.0f, -2.0f, -2.0f,  0.0f, 0.0f,
		  2.0f, -2.0f, -2.0f,  1.0f, 0.0f,
		  2.0f,  2.0f, -2.0f,  1.0f, 1.0f,
		  2.0f,  2.0f, -2.0f,  1.0f, 1.0f,
		  -2.0f,  2.0f, -2.0f,  0.0f, 1.0f,
		  -2.0f, -2.0f, -2.0f,  0.0f, 0.0f, //back

		  -2.0f, -2.0f,  2.0f,  0.0f, 0.0f,
		  2.0f, -2.0f,  2.0f,  1.0f, 0.0f,
		  2.0f,  2.0f,  2.0f,  1.0f, 1.0f,
		  2.0f,  2.0f,  2.0f,  1.0f, 1.0f,
		  -2.0f,  2.0f,  2.0f,  0.0f, 1.0f,
		  -2.0f, -2.0f,  2.0f,  0.0f, 0.0f, //fornt

		  -2.0f,  2.0f,  2.0f,  1.0f, 0.0f,
		  -2.0f,  2.0f, -2.0f,  1.0f, 1.0f,
		  -2.0f, -2.0f, -2.0f,  0.0f, 1.0f,
		  -2.0f, -2.0f, -2.0f,  0.0f, 1.0f,
		  -2.0f, -2.0f,  2.0f,  0.0f, 0.0f,
		  -2.0f,  2.0f,  2.0f,  1.0f, 0.0f,

		  2.0f,  2.0f,  2.0f,  1.0f, 0.0f,
		  2.0f,  2.0f, -2.0f,  1.0f, 1.0f,
		  2.0f, -2.0f, -2.0f,  0.0f, 1.0f,
		  2.0f, -2.0f, -2.0f,  0.0f, 1.0f,
		  2.0f, -2.0f,  2.0f,  0.0f, 0.0f,
		  2.0f,  2.0f,  2.0f,  1.0f, 0.0f,

		  -2.0f, -2.0f, -2.0f,  0.0f, 1.0f,
		  2.0f, -2.0f, -2.0f,  1.0f, 1.0f,
		  2.0f, -2.0f,  2.0f,  1.0f, 0.0f,
		  2.0f, -2.0f,  2.0f,  1.0f, 0.0f,
		  -2.0f, -2.0f,  2.0f,  0.0f, 0.0f,
		  -2.0f, -2.0f, -2.0f,  0.0f, 1.0f,

		  -2.0f,  2.0f, -2.0f,  0.0f, 1.0f,
		  2.0f,  2.0f, -2.0f,  1.0f, 1.0f,
		  2.0f,  2.0f,  2.0f,  1.0f, 0.0f,
		  2.0f,  2.0f,  2.0f,  1.0f, 0.0f,
		  -2.0f,  2.0f,  2.0f,  0.0f, 0.0f,
		  -2.0f,  2.0f, -2.0f,  0.0f, 1.0f
		};

		unsigned int VBO;
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}

};

#endif