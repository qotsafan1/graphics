#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex;
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;
uniform sampler2DRect ssao_tex;
uniform sampler2DShadow	shadow_map;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

uniform mat4 Mat;
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
        fragColor  = vec4(0.4,0.35,0.95,0);
    else
        fragColor = texture(ctex, gl_FragCoord.xy);
}
