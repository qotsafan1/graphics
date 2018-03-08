#include "cubemapbuffer.h"

#include <iostream>

using namespace std;
void CubemapBuffer::relinquish()
{
    glDeleteTextures(1, &cubetex);
    glDeleteFramebuffers(1, &fbo);
}

void CubemapBuffer::initialize(int size)
{
    size = size;
    relinquish();

    glDisable(GL_DEPTH_TEST);
    glGenTextures(1, &cubetex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubetex);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

    for(int i = 0; i < 6; i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, size, size, 0, GL_RGB, GL_FLOAT, 0);
    }

    glGenFramebuffers(1,&fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, cubetex, 0);



    if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        cout << "Something wrong with FBO" << endl;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glEnable(GL_DEPTH_TEST);
}

CubemapBuffer::CubemapBuffer(int size): fbo(0), cubetex(0)
{
    initialize(size);
}

GLuint CubemapBuffer::get_texture()
{
    return cubetex;
}

void CubemapBuffer::enable_write()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
}
