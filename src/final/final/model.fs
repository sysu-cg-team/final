#version 330 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;

	vec3 TangentNormal;
	vec3 TangentLightPos;
	vec3 TangentViewPos;
	vec3 TangentFragPos;
} fs_in;

uniform bool hasDiffuseTexture;
uniform bool hasSpecularTexture;
uniform bool hasAmbientTexture;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_ambient1;
uniform sampler2D shadowMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

void main()
{           
	// 用漫反射贴图的颜色作为颜色
	vec3 color;
	if (hasDiffuseTexture) {
		color = texture(texture_diffuse1, fs_in.TexCoords).rgb;
	} else {
		color = vec3(0.5f, 0.5f, 0.5f);
	}
    vec3 normal = normalize(fs_in.TangentNormal);
    vec3 lightColor = vec3(0.4f);

    // ambient
	vec3 ambient;
	if (hasAmbientTexture) {
		ambient = color * texture(texture_ambient1, fs_in.TexCoords).rgb * 0.2;
	} else {
		ambient = color * 0.2;
	}
     
    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
	vec3 spec;
	if (hasSpecularTexture) {
		spec = texture(texture_specular1, fs_in.TexCoords).rgb;
	} else {
		spec = vec3(0.8f);
	}
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 256.0) * spec;
    vec3 specular = spec * lightColor;    

    // calculate shadow
    float shadow;
	if (lightPos.y < 0) {
		shadow = 1;
	} else {
		shadow = ShadowCalculation(fs_in.FragPosLightSpace);  
	}
	
	shadow = min(shadow, 0.75);                    
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    
	float gamma = 2.2;
    FragColor = vec4(pow(lighting.rgb, vec3(1.0/gamma)), 1.0);
}