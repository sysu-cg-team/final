#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
	vec3 Normal;
	vec2 TexCoord;
} fs_in;


uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D mask;
uniform sampler2D shadowMap;
 
in vec3 Pos;

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
    vec3 lightDir = normalize(lightPos);
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
  vec3 color;
  if (Pos.y == 0) {
  	  discard;
  }
  
  if(Pos.y > 20)
  {
    color = texture2D(texture1, fs_in.TexCoord).rgb;
  }
  else
  {
    float alpha = Pos.y/(20);
    color = mix(texture(texture1, fs_in.TexCoord), texture(texture2, fs_in.TexCoord), 1- alpha).rgb;
  }
  float alpha = texture2D(mask, fs_in.TexCoord).r;
  color = color*(1-alpha)+vec3(0.5,0.6, 0)*alpha;

	vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.4);
    // ambient
    vec3 ambient = 0.2 * color;
    // diffuse
    vec3 lightDir = normalize(lightPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 1.0);
    vec3 specular = 0 * lightColor;    
    // calculate shadow
    	float shadow;
	if (lightDir.y < -2) {
		shadow = 1;
	} else {
		shadow = ShadowCalculation(fs_in.FragPosLightSpace);        
	}            
	 shadow = min(shadow, 0.75);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;    
	//FragColor = vec4(lighting, 1.0);
	float gamma = 2.2;
    FragColor = vec4(pow(lighting.rgb, vec3(1.0/gamma)), 1.0);
}