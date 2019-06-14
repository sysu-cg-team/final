#version 330 core

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec4 FragPosLightSpace;
} fs_in;


uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D mask;
uniform sampler2D shadowMap;
 
in vec2 TexCoord;
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

    float bias = 0.008;
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
  vec4 color;
 
  if(Pos.y >50)
  {
    color = texture2D(texture1, TexCoord);
  }
  else
  {
    float alpha = Pos.y/(50);
    color = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1- alpha);
  }
  float shadow = ShadowCalculation(fs_in.FragPosLightSpace);    
  float alpha = texture2D(mask, TexCoord).r;
  color = color*(1-alpha)+vec4(1.0,1.0,0, 0.5)*alpha;
  vec3 ambient = (color * 0.3).xyz;
  FragColor = vec4(ambient, 1) + (1-shadow) * color;
}