OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	gl_FragColor.x	=	gl_FragCoord.z;
	COLOUR_TRANSFER(gl_FragColor);
}

