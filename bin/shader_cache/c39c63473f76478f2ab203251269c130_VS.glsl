OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "FFPLib_Transform.glsl"
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

layout(location = 0) uniform	mat4	worldviewproj_matrix;
layout(location = 4) uniform	mat3	normal_matrix;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vertex, POSITION)
IN(vec3	normal, NORMAL)
OUT(vec3	vsTexcoord_0, 0)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;

	gl_Position	=	mul(worldviewproj_matrix, vertex);
	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	vsTexcoord_0	=	mul(normal_matrix, normal);
	vsTexcoord_0	=	normalize(vsTexcoord_0);
}

