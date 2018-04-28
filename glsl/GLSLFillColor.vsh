
attribute vec4 position;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * position;
}
