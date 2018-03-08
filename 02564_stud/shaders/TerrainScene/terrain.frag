#version 150
#define SOLUTION_CODE
uniform vec4 light_pos;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

in vec3 pos;
in vec3 norm;
in vec3 norm_obj;

const vec4 col_steep = vec4(.75,.7,.75,1.0);
const vec4 col_high = vec4(0.75,.85,.75,0);
const vec4 col_low = vec4(0.0,1,0.5,0);

uniform sampler3D noise_tex;

out vec4 fragColor;

float sqr(float x) {return x*x*x*x;}

void main()
{
        fragColor =  col_low;

}
