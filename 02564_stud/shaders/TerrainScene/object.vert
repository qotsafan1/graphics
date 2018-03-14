#version 150
in vec3 vertex;
in vec3 normal;

out vec3 norm;
out float d;
out vec3 refl_dir;

uniform vec3 light_half_vec;
uniform vec4 light_pos;

// The projection, view model matrix PVM and the normal matrix N
uniform mat4 PVM;
uniform mat3 N;

void main()
{
    norm = normalize(N*normal);
    d = dot(norm,normalize(light_pos.xyz));
    refl_dir = light_half_vec;
    gl_Position = PVM * vec4(vertex,1);
}

