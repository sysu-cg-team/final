#version 330 core
layout (points) in;
layout (triangle_strip, max_vertices = 12) out;

in mat4 matrix[];
in vec3 Pos[];

uniform float Time;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

out GS_OUT {
    vec3 FragPos;
    vec3 Normal;
	vec2 TexCoord;
    vec4 FragPosLightSpace;
} gs_out;

void main() {
  int vertexCount = 12;
  vec4 v[12];
  vec2 uv[12];

  float _Height = 6, _Width = 0.1;
  float random = sin(1.57 * fract(Pos[0].x / 3) + 1.57 * fract(Pos[0].z / 3));
  _Height = _Height + 3 * random;
  _Width = _Width + random / 50;

  _Height = _Height;
  _Width = _Width;
  vec4 root = vec4(Pos[0], 1);
  //处理纹理坐标
  float currentV = 0;
  float offsetV = 1.0f / ((vertexCount / 2) - 1);

  //处理当前的高度
  float currentHeightOffset = 0;
  float currentVertexHeight = 0;

  //风;
  float windCoEff = 0;
  vec2 wind;
  float oscillateDelta = 0.05;
  wind = vec2(sin(Time * 3.14 * 5));
	wind.x += (sin(Time + Pos[0].x / 25) + sin((Time + Pos[0].x / 15) + 50)) * 0.5;
	wind.y += cos(Time + Pos[0].z / 80);
	wind *= 0.7 + 0.3 * smoothstep(0.0, 1.0, 1 - random);
	float oscillationStrength = 2.5f;
	float sinSkewCoeff = random;
	float lerpCoeff = (sin(oscillationStrength * Time + sinSkewCoeff) + 1.0) / 2;
	vec2 leftWindBound = wind * (1.0 - oscillateDelta);
	vec2 rightWindBound = wind * (1.0 + oscillateDelta);

	wind.x = leftWindBound.x +  (rightWindBound.x - leftWindBound.x) * lerpCoeff;
	wind.y = leftWindBound.y +  (rightWindBound.y - leftWindBound.y) * lerpCoeff;

	float randomAngle = 3.14 * random;
	float randomMagnitude = random;
	vec2 randomWindDir = vec2(sin(randomAngle), cos(randomAngle));
	wind += randomWindDir * randomMagnitude;

	float windForce = length(wind); 
  for (int i = 0; i < vertexCount; i++)
  {
    if (mod(i, 2) == 0) {
      v[i] = root + vec4(-_Width, currentVertexHeight, 0, 0);
	  uv[i] = vec2(0, currentV);
    } else {
      v[i] = root + vec4(_Width, currentVertexHeight, 0, 0);
	  uv[i] = vec2(1, currentV);

	  currentV += offsetV;
	  currentVertexHeight = currentV * _Height;
    }

	

	v[i].xz += 0.5 * wind.xy * pow(windCoEff, 2);
	v[i].y -= 0.5 * windForce * pow(windCoEff, 2) * 0.8;

	if (mod(i, 2) == 1) {
		windCoEff += offsetV;
	}


  }

  
  for (int i = 0; i < vertexCount; i++) {
    gl_Position = matrix[0] * v[i];
	gs_out.TexCoord = uv[i];
	gs_out.FragPos = vec3(model * v[i]);
	gs_out.Normal = transpose(inverse(mat3(model))) * vec3(0, 0, 1);
	gs_out.FragPosLightSpace = lightSpaceMatrix * vec4(gs_out.FragPos, 1.0);
    EmitVertex();
  }
  EndPrimitive();
}