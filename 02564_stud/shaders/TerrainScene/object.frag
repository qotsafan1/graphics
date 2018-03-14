#version 150
in vec3 norm;
in float d;
in vec3 refl_dir;
out vec4 fragColor;

// The following uniforms pertain to illumination. These are the standard uniforms
// that are available when you use a ShaderProgramDraw and call set_light_intensities
// and set_light_position.


uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

// The following uniforms pertain to materials. These are the standard uniforms
// that are available when you use a ShaderProgramDraw and call set_material

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;

void main()
{
    vec4 _color = mat_diff*light_amb;

    if(d>0.0)
    {
        _color += mat_diff*d*light_diff;
    }

    float n_dot_h = dot(norm, normalize(refl_dir));

    if(n_dot_h>0.0)
        _color += mat_spec*pow(n_dot_h, mat_spec_exp)*light_spec;

    fragColor = _color;
}

