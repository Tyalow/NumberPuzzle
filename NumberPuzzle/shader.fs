#version 330 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
    vec2 someVec = vec2(texCoord.x, -texCoord.y);
    FragColor = texture(texture1, someVec);
}