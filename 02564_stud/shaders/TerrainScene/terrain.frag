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

uniform sampler2D noise_tex;


out vec4 fragColor;

float sqr(float x) {return x*x*x*x;}
float rand(float n){return fract(sin(n) * 43758.5453123);}

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d // https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners percentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
    // Nice darkgreen grass color
    vec3 f_color = vec3(0.0,0.2,0.0);

    //Just a small variable to make the grass a little darker close to the water
    float water_height = -0.6;

    //Noise opacity
    float opacity = 0.05;

    //How many noise repeats
    float repeats = 8;

    vec3 color = vec3(noise(pos.xy*repeats)*opacity+f_color.x*(sqrt(pos.z-water_height)),                           noise(pos.xy*repeats)*opacity+f_color.y*(sqrt(pos.z-water_height)),                           noise(pos.xy*repeats)*opacity+f_color.z*(sqrt(pos.z-water_height)));

    fragColor = vec4(color,1.0);
}
