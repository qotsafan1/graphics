#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex;
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;
uniform sampler2DRect ssao_tex;
uniform sampler2DShadow	shadow_map;
uniform samplerCube sky_irradiance;
uniform samplerCube sky_texture;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;


const float M_PI = 3.14159265358979323846f;

uniform mat4 Mat;
uniform mat4 ss_view_to_world;
uniform mat4 ss_proj_view_inv;
uniform vec2 win_scale;

const float INV_WIN_SCALING = 0.5;

float sample_shadow_map(vec3 eye_pos)
{
    return 1.0;
}

out vec4 fragColor;
void main()
{
    vec3 p = texture(gtex, gl_FragCoord.xy).xyz;
    vec4 base_color = texture(ctex, gl_FragCoord.xy);
    if(p.z == -1000.0)
    {
        // This just a simple mapping to spherical coordinates to visualize in 2d the whole cubemap.
        // you will need to replace this code accounting for view and projection from the user.
        vec2 uv = win_scale * gl_FragCoord.xy * vec2(2.0f*M_PI,  M_PI);
        vec3 spherical_coordinates = vec3(sin(uv.y)*cos(uv.x), sin(uv.y)*sin(uv.x), -cos(uv.y));
        fragColor  = texture(sky_texture, spherical_coordinates);
    }
    else
    {
        fragColor = texture(ctex, gl_FragCoord.xy);
    }
}
