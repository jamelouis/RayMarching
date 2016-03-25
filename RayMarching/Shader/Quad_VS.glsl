#version 330


layout(location = 0) in vec2 position;

out vec2 texCoord;
void main()
{
    texCoord = (position + 1.0) / 2.0;
    gl_Position = vec4(position, 0.0, 1.0);
}