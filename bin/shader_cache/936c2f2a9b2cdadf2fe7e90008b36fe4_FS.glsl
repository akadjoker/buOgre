OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "SGXLib_IntegratedPSSM.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_NormalMap.glsl"
#include "SGXLib_PerPixelLighting.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(shadow_map2, 2);
SAMPLER2D(shadow_map3, 3);
SAMPLER2D(gNormalMapSampler0, 0);
SAMPLER2D(gTextureSampler1, 1);
layout(location = 0) uniform	vec4	pssm_split_points0;
layout(location = 1) uniform	vec4	inverse_texture_size2;
layout(location = 2) uniform	vec4	inverse_texture_size3;
layout(location = 3) uniform	vec4	derived_ambient_light_colour;
layout(location = 4) uniform	vec4	surface_emissive_colour;
layout(location = 5) uniform	vec4	derived_scene_colour;
layout(location = 6) uniform	vec4	light_position_view_space_array[1];
layout(location = 7) uniform	vec4	light_direction_view_space_array[1];
layout(location = 8) uniform	vec4	light_attenuation_array[1];
layout(location = 9) uniform	vec4	spotlight_params_array[1];
layout(location = 10) uniform	vec4	derived_light_diffuse_colour_array[1];

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec4	vsTexcoord_0, 0)
IN(vec4	vsTexcoord_1, 1)
IN(vec2	vsTexcoord_2, 2)
IN(vec3	vsTexcoord_3, 3)
IN(vec4	vsTexcoord_4, 4)
IN(vec3	vsTexcoord_5, 5)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;
	float	lShadowFactor[2];
	vec3	lTexcoord_3;
	mat3	TBN;
	vec4	texel_1;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	SGX_ShadowPCF4(shadow_map2, vsTexcoord_0, inverse_texture_size2.xy, lShadowFactor[int(0.00000)]);
	SGX_ShadowPCF4(shadow_map3, vsTexcoord_1, inverse_texture_size3.xy, lShadowFactor[int(1.00000)]);
	SGX_CalculateTBN(vsTexcoord_3, vsTexcoord_4, TBN);
	vec2 local_vsTexcoord_2 = vsTexcoord_2;
	SGX_Generate_Parallax_Texcoord(gNormalMapSampler0, local_vsTexcoord_2, vsTexcoord_5, 0.0400000, TBN, local_vsTexcoord_2);
	local_vsTexcoord_2	=	local_vsTexcoord_2;
	SGX_FetchNormal(gNormalMapSampler0, local_vsTexcoord_2, lTexcoord_3);
	lTexcoord_3	=	mul(TBN, lTexcoord_3);
	gl_FragColor	=	derived_scene_colour;
	evaluateLight(lTexcoord_3, vsTexcoord_5, light_position_view_space_array[int(0.00000)], light_attenuation_array[int(0.00000)], light_direction_view_space_array[int(0.00000)], spotlight_params_array[int(0.00000)], derived_light_diffuse_colour_array[int(0.00000)], gl_FragColor.xyz, lShadowFactor[int(0.00000)]);
	lColor_0	=	gl_FragColor;
	texel_1	=	texture2D(gTextureSampler1, local_vsTexcoord_2);
	ENABLE_LINEAR_COLOUR(texel_1);
	gl_FragColor	=	texel_1*gl_FragColor;
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	COLOUR_TRANSFER(gl_FragColor);
}

