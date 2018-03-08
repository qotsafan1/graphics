#version 150

uniform vec4 mat_diff;
uniform vec4 mat_spec;
uniform sampler2D tex;

uniform mat3 N;

in vec3 p;
in vec3 n;

out vec4 fragData[3];

void main()
{
        fragData[0].rgb = p;
        fragData[1].rgb = N * vec3(0,0,1);
        fragData[2] = vec4(0,0.5,0.7,1);
}
