#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform vec2 WIN_SCALE;           // precisely equal to: WINDOW_WIDTH/2.0, WINDOW_HEIGHT/2.0
noperspective out vec3 dist;  // Vec3 containing the distances from a vertex to opposite edge

void main(void)
{
    dist = vec3(50,0,0);
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    dist = vec3(0,50,0);
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    dist = vec3(0,0,50);
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
}
