#version 330

layout (location = 0) in vec3 aPos;
attribute vec3 position;
attribute vec3 normal;

uniform mat4 modelViewMat;
uniform mat4 perspProjMat;
uniform mat3 normalMat;
uniform mat4 modelMat;

uniform float time;
//uniform vec3 lightPos;
//uniform vec3 eyePos;

varying vec2 texture_coord;
varying vec3 normalVect;
varying vec3 lightVect;
varying vec3 eyeVect;
varying vec3 halfWayVect;
varying vec3 reflectVect;

void main()
{
  gl_Position = perspProjMat * modelViewMat * modelMat * vec4(position, 1.0);

  float tex_x = (position.x + time/20.0) / 8.0 + 0.5;
  float tex_y = 0.5 - (position.y + time/25.0) / 5.0;
  texture_coord = vec2(tex_x, tex_y);

  vec3 eyePos = vec3(0.0, 5.0, 0.0);
  vec3 lightPos = vec3(0.0, 10.0, 0.0);
  vec3 ptVertex = vec3(modelViewMat * vec4(position, 1.0));

  eyeVect = normalize(eyePos - ptVertex);
  lightVect = normalize(lightPos - ptVertex);
  halfWayVect = eyeVect + lightVect;
  normalVect = normalMat * normal;
  reflectVect = 1.0 * eyeVect - 2.0 * dot(-1.0*eyeVect, normalVect) * normalVect;
}
