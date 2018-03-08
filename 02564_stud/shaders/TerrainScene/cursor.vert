#version 150
in vec3 vertex;
uniform mat4 PVM;
void main()
{
    gl_Position = PVM * vec4(vertex.xyz, 1);
}
