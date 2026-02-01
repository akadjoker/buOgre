OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "FFPLib_Transform.glsl"
#include "RTSLib_Colour.glsl"
#include "SGXLib_NormalMap.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

layout(location = 0) uniform	mat4	worldviewproj_matrix;
layout(location = 4) uniform	mat3	normal_matrix;
layout(location = 7) uniform	mat4	worldview_matrix;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vertex, POSITION)
IN(vec4	uv0, TEXCOORD0)
IN(vec3	normal, NORMAL)
IN(vec4	tangent, TANGENT)
OUT(vec2	vsTexcoord_0, 0)
OUT(vec3	vsTexcoord_1, 1)
OUT(vec4	vsTexcoord_2, 2)
OUT(vec3	vsTexcoord_3, 3)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;

	gl_Position	=	mul(worldviewproj_matrix, vertex);
	vsTexcoord_0	=	uv0.xy;
	vsTexcoord_1	=	mul(normal_matrix, normal);
	vsTexcoord_1	=	normalize(vsTexcoord_1);
	vsTexcoord_2.xyz	=	mul(normal_matrix, tangent.xyz);
	vsTexcoord_2.xyz	=	normalize(vsTexcoord_2.xyz);
	vsTexcoord_2.w	=	tangent.w;
	FFP_Transform(worldview_matrix, vertex, vsTexcoord_3);
	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
}

