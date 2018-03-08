#version 150

uniform sampler3D noise_tex;

#define SOLUTION_CODE

in vec3 p;
in vec3 pos;
in vec3 n;
in vec3 n_obj;
in vec3 t;

const vec4 col_steep = vec4(.75,.7,.75,1.0);
const vec4 col_high = vec4(0.75,.85,.75,0);
const vec4 col_low = vec4(0.0,1,0.5,0);

float sqr(float x) {return x*x*x*x;}

out vec4 fragData[3];
void main() 
{

        fragData[0].rgb = p;
        fragData[1].rgb = normalize(n);
	// Map height to color
        vec4 base_color =  col_low;
        base_color.w = 0.0;
        fragData[2] = base_color;
;
}
