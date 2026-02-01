OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_LayeredBlending.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(gTextureSampler0, 0);
SAMPLER2D(gTextureSampler1, 1);
layout(location = 0) uniform	vec4	custom2;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec2	vsTexcoord_0, 0)
IN(vec2	vsTexcoord_1, 1)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;
	vec4	texel_0;
	vec4	texel_1;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	texel_0	=	texture2D(gTextureSampler0, vsTexcoord_0);
	ENABLE_LINEAR_COLOUR(texel_0);
	texel_1	=	texture2D(gTextureSampler1, vsTexcoord_1);
	ENABLE_LINEAR_COLOUR(texel_1);
	gl_FragColor	=	texel_0*gl_FragColor;
	SGX_src_mod_inv_modulate(texel_1, custom2, texel_1);
	SGX_blend_luminosity(texel_1, gl_FragColor, gl_FragColor);
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	COLOUR_TRANSFER(gl_FragColor);
}

