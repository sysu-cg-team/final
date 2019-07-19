#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox1;
uniform samplerCube skybox2;
uniform float time;

void main()
{	
	float alpha;
	if (time < 0) {
		alpha = 0;
	} else {
		alpha = time;
	}
    FragColor = mix(texture(skybox1, TexCoords), texture(skybox2, TexCoords), 1 - alpha);
}