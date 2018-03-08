 #version 150
in vec3 vertex;
in vec3 normal;

out vec4 _color;

// The following uniforms pertain to illumination. These are the standard uniforms
// that are available when you use a ShaderProgramDraw and call set_light_intensities
// and set_light_position.
uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

// The following uniforms pertain to materials. These are the standard uniforms
// that are available when you use a ShaderProgramDraw and call set_material
uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform float mat_spec_exp;

// The projection, view model matrix PVM and the normal matrix N
uniform mat4 PVM;
uniform mat3 N;

void main()
{
    vec3 norm = normalize(N*normal);
    float d = dot(norm,normalize(light_pos.xyz));

    _color = mat_diff*light_amb;
    if(d>0.0)
    {
        _color += mat_diff*d*light_diff;
        float n_dot_h = dot(norm, normalize(light_half_vec));
        if(n_dot_h>0)
            _color += mat_spec*pow(n_dot_h, mat_spec_exp)*light_spec;
    }

    gl_Position = PVM * vec4(vertex,1);
}
