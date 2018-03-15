#version 150
#define SOLUTION_CODE

uniform sampler2DRect gtex;         // GBuffer, geometry texture
uniform sampler2DRect ntex;         // GBuffer, normal texture
uniform sampler2DRect ctex;         // GBuffer, color texture
uniform vec3 disc_points[256];      // Random points in disc
uniform vec2 win_dim;               // Windows dimensions

uniform int NO_DISC_POINTS;         // Number of points in disc.
const float RADIUS = 5.0;           // Eye space radius
const float WIN_RADIUS = 50.0;      // Window space radius
const float WIN_SCALING = 2.0;      // Scaling of window

float sqr(float x) {return x*x;}
out vec4 fragColor;
void main()
{
    const float pi = 3.1415926;
    vec3 p = texture(gtex, WIN_SCALING*gl_FragCoord.xy).xyz;
    if(p.z == -1000.0)
        fragColor = vec4(1.0);
    else
    {
        vec3 n = texture(ntex, WIN_SCALING*gl_FragCoord.xy).xyz;

        float sum = 0.0;

        for (int i=0; i < NO_DISC_POINTS; i++)
        {
            vec2 G = WIN_SCALING*gl_FragCoord.xy + WIN_RADIUS*disc_points[i].xy;
            vec3 vi = texture(gtex, G).xyz - p;
            sum += (max(0, 1-(length(vi)/RADIUS)) * max(0, dot(n,(vi/length(vi)))));
        }

        float ambient_occlusion= 1 - sum/NO_DISC_POINTS;
        fragColor = vec4(ambient_occlusion,ambient_occlusion,ambient_occlusion,1);
        //fragColor = vec4(.5,.5,.5,0);
    }
}
