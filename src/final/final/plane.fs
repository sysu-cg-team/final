#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D mask;
 
in vec2 TexCoord;
in vec3 Pos;
 
void main()
{
  vec4 color;
 
  if(Pos.y >100)
  {
    color = texture2D(texture1, TexCoord);
  }
  else
  {
    float alpha = Pos.y/(100);
    color = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 1- alpha);
  }
 
  float alpha = texture2D(mask, TexCoord).r;
  FragColor = color*(1-alpha)+vec4(1.0,1.0,0, 0.5)*alpha;
}