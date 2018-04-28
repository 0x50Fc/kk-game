#include "kk-config.h"
#include "GLSLFillColor.h"

namespace kk {
	namespace GLSL {
		kk::GL::Program * GLSLFillColorCreate(){ 
			return new kk::GL::Program("\nattribute vec4 position;\n\nuniform mat4 projection;\nuniform mat4 view;\n\nvoid main()\n{\n    gl_Position = projection * view * position;\n}\n","\n#ifdef GL_ES\nprecision lowp float;\n#endif\n\nuniform vec4 color;\n\nvoid main()\n{\n    gl_FragColor =  color;\n}\n");
		}
	}
}
