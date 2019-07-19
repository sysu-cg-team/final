#ifndef GRASS_H
#define GRASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>

#include <stb_image.h>

#include "object.h"

class Grass : public Object
{
public:
	Grass(glm::vec3 pos, glm::vec3 size, glm::vec3 color, int num_row, int num_col, float interval) : Object(pos, size, color)
	{
		this->interval = interval;
		this->num_col = num_col;
		this->num_row = num_row;
		this->InitRenderData();
	};
	~Grass() {};

	float interval;
	int num_row;
	int num_col;
	unsigned int VAO;
	int size;

	void Draw(Shader *shader)
	{
		shader->use();
		glm::mat4 model(1.0f);
		model = glm::translate(model, this->Position);
		model = glm::scale(model, this->Size);
		shader->setMat4("model", model);
		auto texture1 = ResourceManager::GetTexture("t_grass");
		glActiveTexture(GL_TEXTURE0);
		texture1.Bind();
		auto texture2 = ResourceManager::GetTexture("a_grass");
		glActiveTexture(GL_TEXTURE1);
		texture2.Bind();
		glBindVertexArray(this->VAO);
		glDrawArrays(GL_POINTS, 0, this->size);
		glBindVertexArray(0);
	}

	void InitRenderData()
	{
		int width, height;
		//unsigned char *image = stbi_load(file, &width, &height, 0, STBI_grey);
		// Now generate texture
		std::vector<glm::vec3> vertices;
		
		for (int i = 0; i < num_row; i++)
		{
			for (int j = 0; j < num_col; j++)
			{
				//glm::vec3 v1(k + 0, static_cast<float>(image[k + i * height]), i + 0);
				/*if (v1.y < 10 && v1.y > 0)
					vertices.push_back(v1);*/
				glm::vec3 v1(j * interval, 0, i * interval);
				vertices.push_back(v1);
			}
		}
		

		this->size = vertices.size();

		//stbi_image_free(image);

		unsigned int VBO;
		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
	}
};

#endif