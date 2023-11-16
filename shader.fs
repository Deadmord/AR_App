#version 460 core
  
in vec3 ourColor;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D textureImg;
uniform vec3 colorMask;
//uniform sampler2D texture2;

void main()
{
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.85) * vec4(ourColor, 1.0);
    //FragColor = mix(texture(texture, TexCoord), vec4(colorMask, 1.0), 0.50) * vec4(ourColor, 1.0);
    FragColor = texture(textureImg, TexCoord) * vec4(ourColor, 1.0) * vec4(colorMask, 1.0);
}