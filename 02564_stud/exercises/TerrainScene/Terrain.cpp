/*
 *  terrain.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <vector>

#include <GLGraphics/GLHeader.h>

#define SOLUTION_CODE

#include "auxil.h"
#include "Terrain.h"

#define USE_VERTEX_ARRAY 1

using namespace CGLA;
using namespace GLGraphics;
using namespace std;

float Terrain::height(const Vec3f & p) const
{
    float x = delta*p[0]+x0;
    float y = delta*p[1]+y0;
    return A*turbulence(x,y);
}

Vec3f Terrain::normal(const CGLA::Vec3f& p) const
{
    Vec3f xvec(2,0,(height(p+Vec3f(1,0,0))-height(p-Vec3f(1,0,0)))/2.0);
    Vec3f yvec(0,2,(height(p+Vec3f(0,1,0))-height(p-Vec3f(0,1,0)))/2.0);
    return normalize(cross(xvec, yvec));
}


void Terrain::draw(ShaderProgramDraw& shader_prog)
{
    shader_prog.set_model_matrix(identity_Mat4x4f());
    static bool was_here_tex = false;
    static GLuint noise_tex = 0;
    if(!was_here_tex)
    {
        // Create texture at this point
        was_here_tex = true;
    }
    shader_prog.use_texture(GL_TEXTURE_3D, "noise_tex", noise_tex,1);

    static GLuint VAO=0;
    static GLuint buffers[2];
    static bool was_here=false;
    static size_t NUM_VERTICES=-1;
    if(VAO == 0)
    {
        const int GridXMin = 200;
        const int GridYMin = 200;
        const int GridXMax = 300;
        const int GridYMax = 300;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        was_here = true;
        glGenBuffers(2, buffers);

        vector<Vec3f> vertices;
        vector<Vec3f> normals;

        for(int j=GridYMin;j<GridYMax;++j)
        {
            normals.push_back(normal(Vec3f(GridXMin,j+1,0)));
            vertices.push_back(Vec3f(GridXMin,j+1,height(Vec3f(GridXMin,j+1,0))));
            for(int i=GridXMin;i<GridXMax;++i)
            {
                normals.push_back(normal(Vec3f(i,j+1,0)));
                vertices.push_back(Vec3f(i,j+1,height(Vec3f(i,j+1,0))));
                normals.push_back(normal(Vec3f(i,j,0)));
                vertices.push_back(Vec3f(i,j,height(Vec3f(i,j,0))));
            }
            normals.push_back(normal(Vec3f(GridXMax-1,j,0)));
            vertices.push_back(Vec3f(GridXMax-1,j,height(Vec3f(GridXMax-1,j,0))));
        }
        glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
        glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vec3f), &vertices[0], GL_STATIC_DRAW);
        GLuint vertex_attrib = shader_prog.get_attrib_location("vertex");
        glEnableVertexAttribArray(vertex_attrib);
        glVertexAttribPointer(vertex_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
        glBufferData(GL_ARRAY_BUFFER, normals.size()*sizeof(Vec3f), &normals[0], GL_STATIC_DRAW);
        GLuint normal_attrib = shader_prog.get_attrib_location("normal");
        glEnableVertexAttribArray(normal_attrib);
        glVertexAttribPointer(normal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);

        NUM_VERTICES = vertices.size();
    }
    else
        glBindVertexArray(VAO);

    glDrawArrays(GL_TRIANGLE_STRIP, 0, static_cast<GLsizei>(NUM_VERTICES));
}
