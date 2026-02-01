OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_TriplanarTexturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(tp_sampler_from_x0, 0);
SAMPLER2D(tp_sampler_from_y1, 1);
SAMPLER2D(tp_sampler_from_z2, 2);
layout(location = 0) uniform	vec3	gTPParams0;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vsColor_0, 0)
IN(vec3	vsTexcoord_0, 1)
IN(vec4	vsTexcoord_1, 2)
void main(void) {
	vec4	lColor_0;

	lColor_0	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	vsColor_0;
	SGX_TriplanarTexturing(gl_FragColor, vsTexcoord_0, vsTexcoord_1, tp_sampler_from_x0, tp_sampler_from_y1, tp_sampler_from_z2, gTPParams0, gl_FragColor);
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_0.xyz;
	COLOUR_TRANSFER(gl_FragColor);
}

