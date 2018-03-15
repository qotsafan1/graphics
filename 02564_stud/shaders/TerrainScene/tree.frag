#version 150
uniform sampler2D tex; // Uniform specifying the texture unit

#define SOLUTION_CODE
in vec4 _color;
in vec3 _texcoord;

out vec4 fragColor;

void main()
{
    fragColor = _color; // * texture(tex, _texcoord.xy);
}
