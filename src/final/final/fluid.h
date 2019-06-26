#ifndef FLUID_H_
#define FLUID_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "util.h"

using namespace std;


const int WAVE_COUNT = 6;
const int STRIP_COUNT = 80;
const int STRIP_LENGTH = 80;
const float START_X = -0.5;
const float START_Y = -0.5;
const float START_Z = -2.5;
const float LENGTH_X = 0.1;
const float LENGTH_Y = 0.1;
const float HEIGHT_SCALE = 0.8;
const int DATA_LENGTH = STRIP_LENGTH * 2 * (STRIP_COUNT - 1);

// file


const GLfloat parameter[6][6] = {
	{ 1.6,	0.12,	0.9,	0.06,	0.0,	0.0 },
	{ 1.3,	0.1,	1.14,	0.09,	0.0,	0.0 },
	{ 0.2,	0.01,	0.8,	0.08,	0.0,	0.0 },
	{ 0.18,	0.008,	1.05,	0.1,	0.0,	0.0 },
	{ 0.23,	0.005,	1.15,	0.09,	0.0,	0.0 },
	{ 0.12,	0.003,	0.97,	0.14,	0.0,	0.0 }
};

const GLint gerstner_sort[6] = {
	0, 0, 1, 1, 1, 1
};

/**
* The following array gerstner_pt_a and gerstner_pt_b represents two waveforms. 
* The first one has comparatively sharp peaks, which is used to draw fine water waves.
* And the second one is wider, which is used to draw long-wavelength water waves.
*/
const  GLfloat gerstner_pt_a[22] = {

	0.0,   0.0,  41.8,  1.4,  77.5,  5.2,  107.6, 10.9,

	132.4, 17.7, 152.3, 25.0, 167.9, 32.4, 179.8, 39.2,

	188.6, 44.8, 195.0, 48.5, 200.0, 50.0

};

const  GLfloat gerstner_pt_b[22] = {

	0.0,   0.0,  27.7,  1.4,  52.9,  5.2,  75.9,  10.8,

	97.2,  17.6, 116.8, 25.0, 135.1, 32.4, 152.4, 39.2,

	168.8, 44.8, 184.6, 48.5, 200.0, 50.0

};

/**
* @brief:Storage time and wavelength, amplitude, direction, frequency and initial coordinates of each wave
*/
struct waves {
	GLfloat time;
	GLfloat wave_length[WAVE_COUNT],
		wave_height[WAVE_COUNT],
		wave_dir[WAVE_COUNT],
		wave_speed[WAVE_COUNT],
		wave_start[WAVE_COUNT * 2];
};

/**
* @brief:Storage data which will be used
*/
struct datas {
	GLuint vertex_buffer, normal_buffer;
	GLuint vertex_shader, fragment_shader, program;
	GLuint diffuse_texture, normal_texture;

	struct {
		GLint diffuse_texture, normal_texture;
	} uniforms;

	struct {
		GLint position;
		GLint normal;
	} attributes;
};

class Fluid {
public:
	GLfloat vertex_data[DATA_LENGTH * 3];
	GLfloat normal_data[DATA_LENGTH * 3];
	GLuint VAO;
	waves water;
	datas dataset;

	Fluid(string vs, string fs, string d_texture, string n_texture);
	~Fluid() {};
	void initWave();
	void initData();
	void calculateWave();
private:
	string diff_texture;
	string norm_texture;
	string fs_filename;
	string vs_filename;
	GLfloat strip[STRIP_COUNT * STRIP_LENGTH * 3];
	GLfloat normal[STRIP_COUNT*STRIP_LENGTH * 3];
	float gerstnerWave(float length, float height, float in, const GLfloat gerstner[22]);
	int normalizeFunc(float in[], float out[], int count);
	static GLuint initTexture(const char *filename);
	static void* readShader(const char *filename, GLint *length);
	static GLuint initShader(GLenum type, const char *filename);
};


#endif
