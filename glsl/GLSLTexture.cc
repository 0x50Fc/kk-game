#include "kk-config.h"
#include "GLSLTexture.h"

namespace kk {
	namespace GLSL {
		kk::GL::Program * GLSLTextureCreate(){ 
			return new kk::GL::Program("\nattribute vec4 position;\nattribute vec2 texCoord;\n\nuniform mat4 projection;\nuniform\tmat4 view;\n\n#ifdef GL_ES\nvarying mediump vec2 vTexCoord;\n#else\nvarying vec2 vTexCoord;\n#endif\n\nvoid main()\n{\n    gl_Position = projection * view * position;\n\tvTexCoord = texCoord;\n}\n","\n#ifdef GL_ES\nprecision lowp float;\n#endif\n\nvarying vec2 vTexCoord;\nuniform sampler2D texture;\n\nvoid main()\n{\n\tgl_FragColor =  texture2D(texture, vTexCoord);\n}\n");
		}
	}
}
