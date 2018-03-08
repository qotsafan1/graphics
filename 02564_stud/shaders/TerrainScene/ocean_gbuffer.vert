#version 150
in vec3 vertex;

out vec3 p;
out vec3 n;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;

void main() {
    gl_Position = PVM * vec4(vertex, 1);
    p = (VM * vec4(vertex, 1)).xyz;
    n = N * vec3(0,0,1);
}
