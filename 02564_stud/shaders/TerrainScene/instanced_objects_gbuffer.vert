#version 150
#define SOLUTION_CODE
in vec3 vertex;
in vec3 normal;

out vec3 p;
out vec3 n;
out vec4 m;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;

void main() 
{
    gl_Position = PVM * vec4(vertex, 1);
    p = (VM * vec4(vertex, 1)).xyz;
    n = N * normal;
}
