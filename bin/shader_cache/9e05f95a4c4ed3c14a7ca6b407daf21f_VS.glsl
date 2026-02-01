OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "FFPLib_Transform.glsl"
#include "RTSLib_Colour.glsl"
#include "SGXLib_PerPixelLighting.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

layout(location = 0) uniform	mat4	worldviewproj_matrix;
layout(location = 4) uniform	mat3	normal_matrix;
layout(location = 7) uniform	vec4	derived_ambient_light_colour;
layout(location = 8) uniform	vec4	surface_emissive_colour;
layout(location = 9) uniform	vec4	derived_scene_colour;
layout(location = 10) uniform	vec4	light_position_view_space_array[1];
layout(location = 11) uniform	vec4	light_attenuation_array[1];
layout(location = 12) uniform	vec4	light_direction_view_space_array[1];
layout(location = 13) uniform	vec4	spotlight_params_array[1];
layout(location = 14) uniform	vec4	derived_light_diffuse_colour_array[1];
layout(location = 15) uniform	mat4	worldview_matrix;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vertex, POSITION)
IN(vec3	normal, NORMAL)
OUT(vec4	vsColor_0, 0)
OUT(vec3	vsTexcoord_0, 1)
OUT(vec4	vsTexcoord_1, 2)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;
	vec3	lTexcoord_2;
	vec3	lTexcoord_3;

	gl_Position	=	mul(worldviewproj_matrix, vertex);
	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	vsColor_0	=	lColor_0;
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	vsTexcoord_0	=	normal;
	vsTexcoord_1	=	vertex;
	lTexcoord_2	=	mul(normal_matrix, normal);
	FFP_Transform(worldview_matrix, vertex, lTexcoord_3);
	vsColor_0	=	derived_scene_colour;
	evaluateLight(lTexcoord_2, lTexcoord_3, light_position_view_space_array[int(0.00000)], light_attenuation_array[int(0.00000)], light_direction_view_space_array[int(0.00000)], spotlight_params_array[int(0.00000)], derived_light_diffuse_colour_array[int(0.00000)], vsColor_0.xyz);
}

