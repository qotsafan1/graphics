#version 150
in vec3 vertex;
in vec3 normal;

uniform mat4 PVM;
uniform mat3 N;

out vec3 pos;
out vec3 norm;
out vec3 norm_obj;

void main()
{
        pos = vertex;
        norm = N * normal;
        norm_obj = normal;
        gl_Position = PVM * vec4(vertex, 1);
}
