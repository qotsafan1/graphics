/*
 *  terrain.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 02/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __TERRAIN_H__
#define __TERRAIN_H__

#include <CGLA/Vec3f.h>
#include <GLGraphics/User.h>
#include <GLGraphics/ShaderProgram.h>

/** This class represents a terrain. It is very simple: The terrain is synthesized using noise
 and once created we can query height and normal at a given 2D location or render the thing. */
class Terrain: public GLGraphics::HeightMap
{
    float A = 150;
    float delta = 0.035;
    float x0=12.231, y0=7.91;

    const int GridXMin = 0;
    const int GridYMin = 0;
    const int GridXMax = 512;
    const int GridYMax = 512;

public:

    /// Construct terrain with amplitude A and scaling delta.
    Terrain() {}

    /// Returns height at given 2D location
    float height(const CGLA::Vec3f&) const;

    /// Returns normal at given 2D location
    CGLA::Vec3f normal(const CGLA::Vec3f&) const;

    /// Draw using OpenGL: Assumes a reasonable shading program is used.
    void draw(GLGraphics::ShaderProgramDraw&);

    // The dummy functions below don't do anything but allow the terrain to be a
    // replacement for an editable terrain.
    void edit(const CGLA::Vec3f&, float amplitude, int n=5) {}
    void smooth(const CGLA::Vec3f&, float amplitude, int n=5) {}
    void reinit() {}
    void smooth() {}
    CGLA::Vec3f ray_trace(const CGLA::Vec3f &p0, const CGLA::Vec3f &dir) const {}

};

#endif
