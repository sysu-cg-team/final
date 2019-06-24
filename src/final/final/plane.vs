#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;

out vec3 Pos;

out VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
	vec3 Normal;
	vec2 TexCoord;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);

	// shadow
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
	vs_out.Normal = transpose(inverse(mat3(model))) * aNormal;
    Pos = aPos;
    vs_out.TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}