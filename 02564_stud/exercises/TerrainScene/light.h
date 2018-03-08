#ifndef LIGHT_H
#define LIGHT_H

#include <CGLA/Vec4f.h>
#include "lib02564/GLGraphics/ShaderProgram.h"

struct DirectionalLight
{
    DirectionalLight() :
        direction(0.0f,0.0f,1.0f),
        diffuse(0.0f),
        ambient(0.0f),
        specular(0.0f)
    {}

    DirectionalLight(const CGLA::Vec3f& direction, const CGLA::Vec4f& diffuse, const CGLA::Vec4f& specular, const CGLA::Vec4f& ambient) :
        direction(direction),
        diffuse(diffuse),
        ambient(ambient),
        specular(specular)
    {}

    CGLA::Vec3f direction;
    CGLA::Vec4f diffuse;
    CGLA::Vec4f ambient;
    CGLA::Vec4f specular;

    void set_in_shader(GLGraphics::ShaderProgramDraw& shader)
    {
        shader.set_light_position(CGLA::Vec4f(direction, 0.0f));
        shader.set_light_intensities(diffuse, specular, ambient);
    }
};

#endif // LIGHT_H
