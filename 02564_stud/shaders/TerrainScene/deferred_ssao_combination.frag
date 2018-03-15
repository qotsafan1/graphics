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

uniform mat4 Mat; //light projection matrix
const float INV_WIN_SCALING = 0.5;

float sample_shadow_map(vec3 eye_pos)
{
    vec4 wpos = Mat*vec4(eye_pos.xyz, 1.0);

    if (texture(shadow_map, vec3(wpos.xy, wpos.z-0.0001)) < wpos.z)
    {
        return 0.2;
    }
    else
    {
        return 1.0;
    }
}

out vec4 fragColor;
void main()
{
    vec4 base_color = texture(ctex, gl_FragCoord.xy);
    vec3 position = texture(gtex, gl_FragCoord.xy).xyz;

    vec3 light_dir = normalize(light_pos.a > 0.0 ? light_pos.xyz - position : light_pos.xyz);

    float shadow = sample_shadow_map(position);

    vec4 mat_diff = vec4(texture(ctex, gl_FragCoord.xy).xyz, 1.0);
    vec3 norm = texture(ntex, gl_FragCoord.xy).xyz;

    float specular = base_color.w * shadow;
    float cos_theta = max(dot(norm, light_dir), 0.0);

    float z = texture(gtex, gl_FragCoord.xy).z;

    if(z == -1000.0)
        fragColor  = vec4(0.4,0.3,1,0);
    else
    {
        // ambient and diffuse part
        vec4 ao = texture(ssao_tex, INV_WIN_SCALING * gl_FragCoord.xy);
        vec4 color = mat_diff*(ao*light_amb + cos_theta*light_diff * shadow);

        vec3 refl_dir = reflect(normalize(position.xyz), norm);

        float r_dot_l = step(0.5, dot(refl_dir, light_dir));

        color += vec4(0.5,0.5,0.5,1.0)*pow(r_dot_l, max(specular, 1.0))*light_spec;


        fragColor = color;
    }
}
