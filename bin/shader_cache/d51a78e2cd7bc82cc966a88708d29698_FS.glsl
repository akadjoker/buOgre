OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_NormalMap.glsl"
#include "FFPLib_Transform.glsl"
#include "SGXLib_CookTorrance.glsl"
#include "RTSLib_IBL.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(gNormalMapSampler2, 2);
SAMPLER2D(metalRoughnessSampler3, 3);
SAMPLER2D(dfgLUTSampler4, 4);
SAMPLERCUBE(iblEnvSampler5, 5);
SAMPLER2D(gTextureSampler0, 0);
SAMPLER2D(gTextureSampler1, 1);
layout(location = 0) uniform	vec4	derived_scene_colour;
layout(location = 1) uniform	vec4	surface_diffuse_colour;
layout(location = 2) uniform	vec4	light_position_view_space_array[1];
layout(location = 3) uniform	vec4	light_diffuse_colour_power_scaled_array[1];
layout(location = 4) uniform	vec4	light_attenuation_array[1];
layout(location = 5) uniform	vec4	spotlight_params_array[1];
layout(location = 6) uniform	vec4	light_direction_view_space_array[1];
layout(location = 7) uniform	float	luminance0;
layout(location = 8) uniform	vec4	texture_size5;
layout(location = 9) uniform	mat4	inverse_view_matrix;

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
	vec2	metalRoughness;
	vec4	mrSample;
	vec4	litResult;
	vec3	baseColor;
	PixelParams	pixel;
	vec4	texel_0;
	vec4	texel_1;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	SGX_CalculateTBN(vsTexcoord_1, vsTexcoord_2, TBN);
	SGX_FetchNormal(gNormalMapSampler2, vsTexcoord_0, lTexcoord_2);
	lTexcoord_2	=	mul(TBN, lTexcoord_2);
	texel_0	=	texture2D(gTextureSampler0, vsTexcoord_0);
	ENABLE_LINEAR_COLOUR(texel_0);
	texel_1	=	texture2D(gTextureSampler1, vsTexcoord_0);
	ENABLE_LINEAR_COLOUR(texel_1);
	texel_1	=	texture2D(gTextureSampler1, vsTexcoord_0);
	ENABLE_LINEAR_COLOUR(texel_1);
	gl_FragColor	=	texel_0*gl_FragColor;
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	mrSample	=	texture2D(metalRoughnessSampler3, vsTexcoord_0);
	metalRoughness	=	mrSample.yz;
	baseColor	=	surface_diffuse_colour.xyz*gl_FragColor.xyz;
	gl_FragColor.xyz	=	vec3(0.00000,0.00000,0.00000);
	gl_FragColor.w	=	surface_diffuse_colour.w*gl_FragColor.w;
	PBR_MakeParams(baseColor, metalRoughness, pixel);
	evaluateIBL(pixel, lTexcoord_2, vsTexcoord_3, inverse_view_matrix, dfgLUTSampler4, iblEnvSampler5, texture_size5.w, luminance0, gl_FragColor.xyz);
	PBR_Lights(lTexcoord_2, vsTexcoord_3, derived_scene_colour, light_position_view_space_array, light_diffuse_colour_power_scaled_array, light_attenuation_array, light_direction_view_space_array, spotlight_params_array, pixel, gl_FragColor.xyz);
	gl_FragColor.xyz	=	texel_1.xyz+gl_FragColor.xyz;
	gl_FragColor.w	=	texel_1.w*gl_FragColor.w;
	COLOUR_TRANSFER(gl_FragColor);
}

