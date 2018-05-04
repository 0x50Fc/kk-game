
#ifdef GL_ES
precision lowp float;
#endif

varying vec2 vTexCoord;
uniform sampler2D texture;
uniform float opacity;

void main()
{
	gl_FragColor =  texture2D(texture, vTexCoord) * opacity;
}
