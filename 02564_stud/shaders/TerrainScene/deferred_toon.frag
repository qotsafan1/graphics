#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex;
uniform sampler2DRect ntex;
uniform sampler2DRect ctex;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;

float sqr(float x) {return x*x;}

out vec4 fragColor;
void main()
{
        float z = texture(gtex, gl_FragCoord.xy).z;
        vec4 base_color = texture(ctex, gl_FragCoord.xy);
        if(z == -1000.0)
                fragColor  = vec4(0.4,0.3,1,0);
        else
        {
            fragColor = base_color;
        }
}
