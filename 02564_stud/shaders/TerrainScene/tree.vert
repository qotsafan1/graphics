#version 150
in vec3 vertex;
in vec3 normal;
in vec3 texcoord;
in int gl_InstanceID;

#define SOLUTION_CODE
out vec4 _color;

uniform vec4 light_pos;
uniform vec3 light_half_vec;
uniform vec4 light_diff;
uniform vec4 light_spec;
uniform vec4 light_amb;
uniform vec4 mat_diff;
uniform float mat_spec_exp;

out vec3 _texcoord;

uniform mat4 PVM;
uniform mat4 VM;
uniform mat3 N;
uniform mat4 InstanceMatrix;

void main()
{
    vec3 norm = normalize(N*vec3((InstanceMatrix*vec4(normal,1)).xyz));
    vec3 position = (VM * InstanceMatrix * vec4(vertex,1)).xyz;
    vec3 light_dir = normalize(light_pos.a > 0.0 ? light_pos.xyz - position : light_pos.xyz);

    float cos_theta = max(dot(norm, light_dir), 0.0);

    // ambient and diffuse part
    _color = mat_diff*(light_amb + cos_theta*light_diff);

    // _texcoord = texcoord;
    gl_Position = PVM * InstanceMatrix * vec4(vertex,1);
}
