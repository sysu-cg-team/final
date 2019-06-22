#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;

	vec3 TangentNormal;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

uniform bool hasNormalTexture;
uniform sampler2D texture_normal1;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vs_out.Normal = normalMatrix * aNormal;
	if (hasNormalTexture) {
		vs_out.TangentNormal = texture(texture_normal1, aTexCoords).rgb;
		vs_out.TangentNormal = vs_out.TangentNormal * 2.0 - 1.0;

		vec3 T = normalize(normalMatrix * aTangent);
		vec3 N = normalize(normalMatrix * aNormal);
		T = normalize(T - dot(T, N) * N);
		vec3 B = cross(N, T);

		mat3 TBN = transpose(mat3(T, B, N));
		vs_out.TangentLightPos = TBN * lightPos;
		vs_out.TangentViewPos = TBN * viewPos;
		vs_out.TangentFragPos = TBN * vs_out.FragPos;
	} else {
		vs_out.TangentNormal = vs_out.Normal;
		vs_out.TangentLightPos = lightPos;
		vs_out.TangentViewPos = viewPos;
		vs_out.TangentFragPos = vs_out.FragPos;
	}
    
    vs_out.FragPosLightSpace = lightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}