#version 150

in vec3 vertex;
in vec3 normal;
in vec3 texcoord;

out vec3 p;
out vec3 pos;
out vec3 n;
out vec3 n_obj;
out vec3 t;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;

void main() 
{
    gl_Position = PVM * vec4(vertex, 1);
    pos = vertex;
    p = (VM * vec4(vertex, 1)).xyz;
    t = texcoord;
    n = normalize(N * normal);
    n_obj = normal;
}
