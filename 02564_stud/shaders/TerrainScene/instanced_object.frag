#version 150
#define SOLUTION_CODE
in vec3 _normal;
in vec4 _mat_diff;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;


out vec4 fragColor;

void main()
{
    fragColor = vec4(1.0);
}
