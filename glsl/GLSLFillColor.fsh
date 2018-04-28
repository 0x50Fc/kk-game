
#ifdef GL_ES
precision lowp float;
#endif

uniform vec4 color;

void main()
{
    gl_FragColor =  color;
}
