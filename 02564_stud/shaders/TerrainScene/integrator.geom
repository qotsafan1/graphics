#version 150
layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

flat out int layer; // We need the layer number in the fragment shader. It's not automatic until #version 430

#define SOLUTION_CODE

void main(void)
{
    for(int i = 0; i < 3; i++)
    {
        gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}
