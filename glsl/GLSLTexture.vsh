
attribute vec4 position;
attribute vec2 texCoord;

uniform mat4 projection;
uniform	mat4 view;

#ifdef GL_ES
varying mediump vec2 vTexCoord;
#else
varying vec2 vTexCoord;
#endif

void main()
{
    gl_Position = projection * view * position;
	vTexCoord = texCoord;
}
