OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_NormalMap.glsl"
#include "SGXLib_PerPixelLighting.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(gNormalMapSampler0, 0);
layout(location = 0) uniform	vec4	derived_ambient_light_colour;
layout(location = 1) uniform	vec4	surface_emissive_colour;
layout(location = 2) uniform	vec4	derived_scene_colour;
layout(location = 3) uniform	float	surface_shininess;
layout(location = 4) uniform	vec4	light_position_view_space_array[2];
layout(location = 6) uniform	vec4	light_direction_view_space_array[2];
layout(location = 8) uniform	vec4	light_attenuation_array[2];
layout(location = 10) uniform	vec4	spotlight_params_array[2];
layout(location = 12) uniform	vec4	derived_light_diffuse_colour_array[2];
layout(location = 14) uniform	vec4	derived_light_specular_colour_array[2];

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec2	vsTexcoord_0, 0)
IN(vec3	vsTexcoord_1, 1)
IN(vec4	vsTexcoord_2, 2)
IN(vec3	vsTexcoord_3, 3)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;
	vec3	lTexcoord_2;
	mat3	TBN;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	SGX_CalculateTBN(vsTexcoord_1, vsTexcoord_2, TBN);
	SGX_FetchNormal(gNormalMapSampler0, vsTexcoord_0, lTexcoord_2);
	lTexcoord_2	=	mul(TBN, lTexcoord_2);
	gl_FragColor	=	derived_scene_colour;
	evaluateLight(lTexcoord_2, vsTexcoord_3, light_position_view_space_array[int(0.00000)], light_attenuation_array[int(0.00000)], light_direction_view_space_array[int(0.00000)], spotlight_params_array[int(0.00000)], derived_light_diffuse_colour_array[int(0.00000)], gl_FragColor.xyz, derived_light_specular_colour_array[int(0.00000)], surface_shininess, lColor_1.xyz);
	evaluateLight(lTexcoord_2, vsTexcoord_3, light_position_view_space_array[int(1.00000)], light_attenuation_array[int(1.00000)], light_direction_view_space_array[int(1.00000)], spotlight_params_array[int(1.00000)], derived_light_diffuse_colour_array[int(1.00000)], gl_FragColor.xyz, derived_light_specular_colour_array[int(1.00000)], surface_shininess, lColor_1.xyz);
	lColor_0	=	gl_FragColor;
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	COLOUR_TRANSFER(gl_FragColor);
}

