#version 150
#define SOLUTION_CODE

flat in int layer;

out vec4 fragColor;

uniform vec2 inv_cubemap_size;
uniform int N; // Monte Carlo samples
uniform int iteration;

uniform samplerCube sky_texture;
uniform samplerCube old_texture;

const float M_PI = 3.14159265358979323846f;

vec3 CUBEMAP_DIRS[6]= vec3[](vec3(1,0,0),vec3(-1,0,0),vec3(0,1,0),vec3(0,-1,0),vec3(0,0,1),vec3(0,0,-1));
vec3 UPS[6] = vec3[](vec3(0,-1,0),vec3(0,-1,0),vec3(0,0,1),vec3(0,0,-1),vec3(0,-1,0),vec3(0,-1,0));
vec3 BASE[6] = vec3[] (vec3(0,0,-1),vec3(0,0,1),vec3(1,0,0),vec3(1,0,0),vec3(1,0,0),vec3(-1,0,0));

/*
    Converts a pixel in pixel coordinates to a vector in a cubemap.
    Note that the Up vectors for the side faces are reversed because Opengl follows the
    RenderMan convention for cubemaps.
    Note that the pixel itself is not enough,we need to know the face the pixel is lying on.
    The order of layer is assumed the opengl one (+x,-x,+y,-y,+z,-z)
*/
vec3 pixel_to_vector_in_cubemap(vec2 pixel, int layer)
{
    vec2 real_coord = 2.0f*pixel*inv_cubemap_size - vec2(1.0f);
    return normalize(CUBEMAP_DIRS[layer] + BASE[layer]*real_coord.x + UPS[layer]*real_coord.y);
}

/*
    Random seed generator
    Usually ment to generate the first seed for a lcg starting from pixel coordinates
*/
uint tea( float val0, float val1 )
{
    uint v0 = uint(val0);
    uint v1 = uint(val1);
    uint s0 = 0u;

    for(int n = 0; n < 4; n++ )
    {
        s0 += 0x9e3779b9u;
        v0 += ((v1<<4)+0xa341316cu)^(v1+s0)^((v1>>5)+0xc8013ea4u);
        v1 += ((v0<<4)+0xad90777du)^(v0+s0)^((v0>>5)+0x7e95761eu);
    }
    return v0;
}

/*
    LCG generator on GPU
*/
uint lcg(inout uint prev)
{
    const uint LCG_A = 1664525u;
    const uint LCG_C = 1013904223u;
    prev = (LCG_A * prev + LCG_C);
    return prev & 0x00FFFFFFu;
}

/*
    Returns a random in [0,1) given a seed from lcg(seed)
*/
float rnd(inout uint prev)
{
    return(float(lcg(prev)) / float(0x01000000));
}

/*
    Rotates a vector towards a normal. The vector v gets modified to newV so that the angle between
    v  and (0,0,1) is the same as newV and n.
*/
void rotate_to_normal(in vec3 normal, inout vec3 v)
{
    if(normal.z < -0.999999f)
    {
        v = vec3(-v.y, -v.x, -v.z);
        return;
    }
    float a = 1.0f/(1.0f + normal.z);
    float b = -normal.x*normal.y*a;
    v = vec3(1.0f - normal.x*normal.x*a, b, -normal.x)*v.x
        + vec3(b, 1.0f - normal.y*normal.y*a, -normal.y)*v.y
        + normal*v.z;
}

/*
    Sample hemisphere: from normal returns a random vector with cosine distribution
*/
vec3 sample_hemisphere_cosine(in vec2 randoms, in vec3 normal)
{
    // Implement me: cosine sampling
    return normal;
}

void main(void)
{
    vec3 current_vector = pixel_to_vector_in_cubemap(gl_FragCoord.xy, layer);
    float width = 1.0f/inv_cubemap_size.x;
    uint t = tea(gl_FragCoord.x + gl_FragCoord.y*width, layer + 6*iteration); // use t as input for rnd to get a random number in [0,1)

    vec4 color = vec4(0.0f);

    // Implement me: integrate sky_texture for N steps using hemisphere sampling

    // Progressive evaluation to avoid display driver time-out and recovery
    vec4 old_color = iteration == 0 ? vec4(0.0f) : texture(old_texture, current_vector)*iteration;
    fragColor = (color + old_color)/(iteration + 1);

    //fragColor = texture(sky_texture, current_vector);
}
