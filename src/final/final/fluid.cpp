#include "fluid.h"

Fluid::Fluid(string vs, string fs, string d_texture, string n_texture) {
	diff_texture = d_texture;
	norm_texture = n_texture;
	fs_filename = fs;
	vs_filename = vs;
	initWave();
	initData();
}

void Fluid::initWave() {
	// initialize structured array
	water.time = 0.0;
	for (int i = 0; i < WAVE_COUNT; i++) {
		water.wave_length[i] = parameter[i][0];
		water.wave_height[i] = parameter[i][1];
		water.wave_dir[i] = parameter[i][2];
		water.wave_speed[i] = parameter[i][3];
		water.wave_start[i * 2] = parameter[i][4];
		water.wave_start[i * 2 + 1] = parameter[i][5];
	}
	// calculate the vertex data of the water surface to be constructed
	int index = 0;
	for (int i = 0; i < STRIP_COUNT; i++)
	{
		for (int j = 0; j<STRIP_LENGTH; j++)
		{
			strip[index] = START_X + i * LENGTH_X;
			strip[index + 1] = START_Y + j * LENGTH_Y;
			index += 3;
		}
	}
}

/**
* @brief:Linear interpolation function
*/
float Fluid::gerstnerWave(float length, float height, float in, const GLfloat gerstner[22])
{
	in = in * 400.0 / length;

	while (in < 0.0)
		in += 400.0;
	while (in > 400.0)
		in -= 400.0;
	if (in > 200.0)
		in = 400.0 - in;

	int i = 0;
	float yScale = height / 50.0;
	while (i < 18 && (in < gerstner[i] || in >= gerstner[i + 2]))
		i += 2;
	if (in == gerstner[i])
		return gerstner[i + 1] * yScale;
	if (in > gerstner[i])
		return ((gerstner[i + 3] - gerstner[i + 1]) * (in - gerstner[i]) / (gerstner[i + 2] - gerstner[i]) + gerstner[i + 3]) * yScale;
}

/* 
* @brief:normalized normal vector (make its length 1)
*/
int Fluid::normalizeFunc(float in[], float out[], int count)
{
	int t = 0;
	float l = 0.0;

	if (count <= 0.0) {
		printf("normalizeF(): Number of dimensions should be larger than zero.\n");
		return 1;
	}
	while (t<count && in[t] < 0.0000001 && in[t] > -0.0000001) {
		t++;
	}
	if (t == count) {
		printf("normalizeF(): The input vector is too small.\n");
		return 1;
	}
	for (t = 0; t<count; t++)
		l += in[t] * in[t];
	if (l < 0.0000001) {
		l = 0.0;
		for (t = 0; t<count; t++)
			in[t] *= 10000.0;
		for (t = 0; t<count; t++)
			l += in[t] * in[t];
	}
	l = sqrt(l);
	for (t = 0; t<count; t++)
		out[t] /= l;

	return 0;
}

void Fluid::calculateWave()
{
	// calculate strip[z], poly_normal[] and normal[]
	int index = 0;
	float d, wave;
	for (int i = 0; i < STRIP_COUNT; i++)
	{
		for (int j = 0; j < STRIP_LENGTH; j++)
		{
			wave = 0.0;
			for (int w = 0; w<WAVE_COUNT; w++) {
				d = (strip[index] - water.wave_start[w * 2] + (strip[index + 1] - water.wave_start[w * 2 + 1]) * tan(water.wave_dir[w])) * cos(water.wave_dir[w]) * cos(water.wave_dir[w]);
				if (gerstner_sort[w] == 1) {
					wave += water.wave_height[w] - gerstnerWave(water.wave_length[w], water.wave_height[w], d + water.wave_speed[w] * water.time, gerstner_pt_a);
				}
				else {
					wave += water.wave_height[w] - gerstnerWave(water.wave_length[w], water.wave_height[w], d + water.wave_speed[w] * water.time, gerstner_pt_b);
				}
			}
			strip[index + 2] = START_Z + wave * HEIGHT_SCALE;
			index += 3;
		}
	}

	index = 0;
	for (int i = 0; i < STRIP_COUNT; i++)
	{
		for (int j = 0; j < STRIP_LENGTH; j++)
		{
			int p0 = index - STRIP_LENGTH * 3, 
				p1 = index + 3, 
				p2 = index + STRIP_LENGTH * 3, 
				p3 = index - 3;
			float xa, ya, za, xb, yb, zb;
			if (i > 0) {
				if (j > 0) {
					xa = strip[p0] - strip[index], ya = strip[p0 + 1] - strip[index + 1], za = strip[p0 + 2] - strip[index + 2];
					xb = strip[p3] - strip[index], yb = strip[p3 + 1] - strip[index + 1], zb = strip[p3 + 2] - strip[index + 2];
					normal[index] += ya * zb - yb * za;
					normal[index + 1] += xb * za - xa * zb;
					normal[index + 2] += xa * yb - xb * ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = strip[p1] - strip[index], ya = strip[p1 + 1] - strip[index + 1], za = strip[p1 + 2] - strip[index + 2];
					xb = strip[p0] - strip[index], yb = strip[p0 + 1] - strip[index + 1], zb = strip[p0 + 2] - strip[index + 2];
					normal[index] += ya * zb - yb * za;
					normal[index + 1] += xb * za - xa * zb;
					normal[index + 2] += xa * yb - xb * ya;
				}
			}
			if (i < STRIP_COUNT - 1) {
				if (j > 0) {
					xa = strip[p3] - strip[index], ya = strip[p3 + 1] - strip[index + 1], za = strip[p3 + 2] - strip[index + 2];
					xb = strip[p2] - strip[index], yb = strip[p2 + 1] - strip[index + 1], zb = strip[p2 + 2] - strip[index + 2];
					normal[index] += ya * zb - yb * za;
					normal[index + 1] += xb * za - xa * zb;
					normal[index + 2] += xa * yb - xb * ya;
				}
				if (j < STRIP_LENGTH - 1) {
					xa = strip[p2] - strip[index], ya = strip[p2 + 1] - strip[index + 1], za = strip[p2 + 2] - strip[index + 2];
					xb = strip[p1] - strip[index], yb = strip[p1 + 1] - strip[index + 1], zb = strip[p1 + 2] - strip[index + 2];
					normal[index] += ya * zb - yb * za;
					normal[index + 1] += xb * za - xa * zb;
					normal[index + 2] += xa * yb - xb * ya;
				}
			}
			if (normalizeFunc(&normal[index], &normal[index], 3))
				printf("%d\t%d\n", index / 3 / STRIP_LENGTH, (index / 3) % STRIP_LENGTH);

			index += 3;
		}
	}

	// calculate vertex_data[] according to strip[], and normal_data[] according to normal[]
	int pt;
	for (int c = 0; c < (STRIP_COUNT - 1); c++)
	{
		for (int l = 0; l < 2 * STRIP_LENGTH; l++)
		{
			if (l % 2 == 1) {
				pt = c * STRIP_LENGTH + l / 2;
			}
			else {
				pt = c * STRIP_LENGTH + l / 2 + STRIP_LENGTH;
			}
			index = STRIP_LENGTH * 2 * c + l;
			for (int i = 0; i<3; i++) {
				vertex_data[index * 3 + i] = strip[pt * 3 + i];
				normal_data[index * 3 + i] = normal[pt * 3 + i];
			}
		}
	}
}

GLuint Fluid::initTexture(const char *filename)
{
	int width, height;
	void *pixels = read_tga(filename, &width, &height);
	GLuint texture;

	if (!pixels)
		return 0;

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_BGR, GL_UNSIGNED_BYTE, pixels);

	free(pixels);
	return texture;
}

void* Fluid::readShader(const char *filename, GLint *length)
{
	FILE *f;
	fopen_s(&f, filename, "r");
	void *buffer;

	if (!f) {
		fprintf(stderr, "Unable to open %s for reading\n", filename);
		return NULL;
	}

	fseek(f, 0, SEEK_END);
	*length = ftell(f);
	fseek(f, 0, SEEK_SET);

	buffer = malloc(*length + 1);
	*length = fread(buffer, 1, *length, f);
	fclose(f);
	((char*)buffer)[*length] = '\0';

	return buffer;
}

GLuint Fluid::initShader(GLenum type, const char *filename) 
{
	GLint length;
	GLchar *source = (GLchar *)readShader(filename, &length);
	GLuint shader;
	GLint shader_ok;

	if (!source)
		return 0;

	shader = glCreateShader(type);
	glShaderSource(shader, 1, (const GLchar**)&source, &length);
	free(source);
	glCompileShader(shader);

	glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
	if (!shader_ok) {
		fprintf(stderr, "Failed to compile %s:\n", filename);
		//infoLog(shader, glGetShaderiv, glGetShaderInfoLog);
		glDeleteShader(shader);
		getchar();
	}
	return shader;
}

void Fluid::initData()
{
	dataset.vertex_shader = initShader(GL_VERTEX_SHADER, vs_filename.c_str());
	dataset.fragment_shader = initShader(GL_FRAGMENT_SHADER, fs_filename.c_str());

	GLint program_ok;
	dataset.program = glCreateProgram();
	glAttachShader(dataset.program, dataset.vertex_shader);
	glAttachShader(dataset.program, dataset.fragment_shader);
	glLinkProgram(dataset.program);
	glGetProgramiv(dataset.program, GL_LINK_STATUS, &program_ok);
	if (!program_ok) {
		fprintf(stderr, "Failed to link shader program:\n");
		//infoLog(dataset.program, glGetProgramiv, glGetProgramInfoLog);
		glDeleteProgram(dataset.program);
		getchar();
	}
	glUseProgram(dataset.program);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLfloat materAmbient[] = { 0.1, 0.1, 0.3, 1.0 };
	GLfloat materSpecular[] = { 0.8, 0.8, 0.9, 1.0 };
	GLfloat lightDiffuse[] = { 0.7, 0.7, 0.8, 1.0 };
	GLfloat lightAmbient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat envirAmbient[] = { 0.1, 0.1, 0.3, 1.0 };
	glUniform4fv(glGetUniformLocation(dataset.program, "materAmbient"), 1, materAmbient);
	glUniform4fv(glGetUniformLocation(dataset.program, "materSpecular"), 1, materSpecular);
	glUniform4fv(glGetUniformLocation(dataset.program, "lightDiffuse"), 1, lightDiffuse);
	glUniform4fv(glGetUniformLocation(dataset.program, "lightAmbient"), 1, lightAmbient);
	glUniform4fv(glGetUniformLocation(dataset.program, "lightSpecular"), 1, lightSpecular);
	glUniform4fv(glGetUniformLocation(dataset.program, "envirAmbient"), 1, envirAmbient);

	dataset.attributes.position = glGetAttribLocation(dataset.program, "position");
	glGenBuffers(1, &dataset.vertex_buffer);

	dataset.attributes.normal = glGetAttribLocation(dataset.program, "normal");
	glGenBuffers(1, &dataset.normal_buffer);

	dataset.diffuse_texture = initTexture(diff_texture.c_str());
	dataset.uniforms.diffuse_texture = glGetUniformLocation(dataset.program, "textures[0]");
	glUniform1i(dataset.uniforms.diffuse_texture, 0);

	dataset.normal_texture = initTexture(norm_texture.c_str());
	dataset.uniforms.normal_texture = glGetUniformLocation(dataset.program, "textures[1]");
	glUniform1i(dataset.uniforms.normal_texture, 1);
}