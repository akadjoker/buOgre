OGRE_NATIVE_GLSL_VERSION_DIRECTIVE
//-----------------------------------------------------------------------------
//                         PROGRAM DEPENDENCIES
//-----------------------------------------------------------------------------
#define USE_OGRE_FROM_FUTURE
#include <OgreUnifiedShader.h>
#include "RTSLib_Colour.glsl"
#include "SGXLib_PerPixelLighting.glsl"
#include "FFPLib_Texturing.glsl"
#include "SGXLib_TriplanarTexturing.glsl"

//-----------------------------------------------------------------------------
//                         GLOBAL PARAMETERS
//-----------------------------------------------------------------------------

SAMPLER2D(tp_sampler_from_x0, 0);
SAMPLER2D(tp_sampler_from_y1, 1);
SAMPLER2D(tp_sampler_from_z2, 2);
layout(location = 0) uniform	vec4	derived_ambient_light_colour;
layout(location = 1) uniform	vec4	surface_emissive_colour;
layout(location = 2) uniform	vec4	derived_scene_colour;
layout(location = 3) uniform	vec4	light_position_view_space_array[1];
layout(location = 4) uniform	vec4	light_direction_view_space_array[1];
layout(location = 5) uniform	vec4	light_attenuation_array[1];
layout(location = 6) uniform	vec4	spotlight_params_array[1];
layout(location = 7) uniform	vec4	derived_light_diffuse_colour_array[1];
layout(location = 8) uniform	vec3	gTPParams0;

//-----------------------------------------------------------------------------
//                         MAIN
//-----------------------------------------------------------------------------
IN(vec3	vsTexcoord_0, 0)
IN(vec3	vsTexcoord_1, 1)
IN(vec3	vsTexcoord_2, 2)
IN(vec4	vsTexcoord_3, 3)
void main(void) {
	vec4	lColor_0;
	vec4	lColor_1;

	lColor_0	=	vec4(1.00000,1.00000,1.00000,1.00000);
	lColor_1	=	vec4(0.00000,0.00000,0.00000,0.00000);
	gl_FragColor	=	lColor_0;
	gl_FragColor	=	derived_scene_colour;
	evaluateLight(vsTexcoord_0, vsTexcoord_1, light_position_view_space_array[int(0.00000)], light_attenuation_array[int(0.00000)], light_direction_view_space_array[int(0.00000)], spotlight_params_array[int(0.00000)], derived_light_diffuse_colour_array[int(0.00000)], gl_FragColor.xyz);
	lColor_0	=	gl_FragColor;
	SGX_TriplanarTexturing(gl_FragColor, vsTexcoord_2, vsTexcoord_3, tp_sampler_from_x0, tp_sampler_from_y1, tp_sampler_from_z2, gTPParams0, gl_FragColor);
	gl_FragColor.xyz	=	gl_FragColor.xyz+lColor_1.xyz;
	COLOUR_TRANSFER(gl_FragColor);
}

