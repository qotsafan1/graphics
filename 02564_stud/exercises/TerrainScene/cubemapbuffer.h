#ifndef CUBEMAPBUFFER_H
#define CUBEMAPBUFFER_H

#include "GLGraphics/GLHeader.h"

class CubemapBuffer
{
    GLuint fbo; // The framebuffer object and the render buffer
    GLuint cubetex; // The three textures needed: geometry, normal, color
    int size;
    /// Get rid of resources.
    void relinquish();

public:

    /// Construct with given size
    CubemapBuffer(int size);

    /// Destroy
    ~CubemapBuffer() {relinquish();}

    /** Initialize with width W and height H. Call if, for instance, the window changes
     size and a different size VPLBuffer is needed. */
    void initialize(int size);

    /** Enable rendering to the FBO that is associated with the Cubemap. */
    void enable_write();

    /// Bind the three VPLBuffer textures to the units passed as arguments.
    GLuint get_texture();
};

#endif // CUBEMAPBUFFER_H
