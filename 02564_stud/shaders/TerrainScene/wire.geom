#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec2 WIN_SCALE;           // precisely equal to: WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0
noperspective out vec3 dist;  // Vec3 containing the distances from a vertex to opposite edge

void main(void)
{
    vec2 A = WIN_SCALE * gl_in[0].gl_Position.xy / gl_in[0].gl_Position.w;
    vec2 B = WIN_SCALE * gl_in[1].gl_Position.xy / gl_in[1].gl_Position.w;
    vec2 C = WIN_SCALE * gl_in[2].gl_Position.xy / gl_in[2].gl_Position.w;

    vec2 ab = B-A;
    vec2 ac = C-A;
    vec2 bc = C-B;

    float area = abs(ab.x*bc.y - ab.y*bc.x);

    float a = area/length(bc);
    float b = area/length(ac);
    float c = area/length(ab);

    dist = vec3(a,0,0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dist = vec3(0,b,0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dist = vec3(0,0,c);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}
