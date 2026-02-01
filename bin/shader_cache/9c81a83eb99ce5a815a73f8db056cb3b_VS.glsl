OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "FFPLib_Transform.glsl"
#include "SGXLib_DualQuaternion.glsl"
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

layout(location = 0) uniform	mat3x4	bone_matrix_array_3x4[70];
layout(location = 210) uniform	mat4	worldviewproj_matrix;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vertex, POSITION)
IN(vec3	normal, NORMAL)
IN(vec4	blendIndices, BLENDINDICES)
IN(vec4	blendWeights, BLENDWEIGHT)
void main(void) {
	mat3x4	blendMat;
	vec4	lColor_1;
	vec4	lColor_2;

	blendBonesMat3x4(bone_matrix_array_3x4, blendIndices, blendWeights, blendMat);
	vec4 local_vertex = vertex;
	FFP_Transform(blendMat, local_vertex, local_vertex.xyz);
	local_vertex.w	=	1.00000;
	gl_Position	=	mul(worldviewproj_matrix, local_vertex);
	vec3 local_normal = normal;
	FFP_Transform(blendMat, local_normal, local_normal);
	lColor_1	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_2	=	vec4(0.00000,0.00000,0.00000,0.00000);
}

