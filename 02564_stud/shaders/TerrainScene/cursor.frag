#version 150
uniform float col;
out vec4 fragColor;
void main(void)
{
    fragColor = vec4(1,-col,col,0);
}
