#version 330 core
layout (location = 0) in vec3 aPos;

out vec2 TexCoord;
out vec3 Pos;

out mat4 matrix;



uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;


void main()
{
  matrix = projection * view * model;

  Pos = aPos;
  gl_Position = projection * view * model * vec4(aPos, 1.0f);
}