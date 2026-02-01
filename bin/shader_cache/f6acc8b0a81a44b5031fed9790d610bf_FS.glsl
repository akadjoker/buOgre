OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "FFPLib_Transform.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_CookTorrance.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

layout(location = 0) uniform	vec4	surface_specular_colour;
layout(location = 1) uniform	vec4	derived_scene_colour;
layout(location = 2) uniform	vec4	surface_diffuse_colour;
layout(location = 3) uniform	vec4	light_position_view_space_array[1];
layout(location = 4) uniform	vec4	light_diffuse_colour_power_scaled_array[1];
layout(location = 5) uniform	vec4	light_attenuation_array[1];
layout(location = 6) uniform	vec4	spotlight_params_array[1];
layout(location = 7) uniform	vec4	light_direction_view_space_array[1];

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec2	vsTexcoord_0, 0)
IN(vec3	vsTexcoord_1, 1)
IN(vec3	vsTexcoord_2, 2)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;
	vec2	metalRoughness;
	vec4	litResult;
	vec3	baseColor;
	PixelParams	pixel;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	metalRoughness	=	surface_specular_colour.xy;
	baseColor	=	surface_diffuse_colour.xyz*gl_FragColor.xyz;
	gl_FragColor.xyz	=	vec3(0.00000,0.00000,0.00000);
	gl_FragColor.w	=	surface_diffuse_colour.w*gl_FragColor.w;
	PBR_MakeParams(baseColor, metalRoughness, pixel);
	PBR_Lights(vsTexcoord_2, vsTexcoord_1, derived_scene_colour, light_position_view_space_array, light_diffuse_colour_power_scaled_array, light_attenuation_array, light_direction_view_space_array, spotlight_params_array, pixel, gl_FragColor.xyz);
	COLOUR_TRANSFER(gl_FragColor);
}

