#version 150
#define SOLUTION_CODE
in vec3 vertex;
in vec3 normal;

out vec3 _normal;
out vec4 _mat_diff;

uniform mat4 PVM;
uniform mat3 N;

void main()
{
    gl_Position = PVM * vertex;
}
