/*
 *  aux.h
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __AUX_H__
#define __AUX_H__

#include <CGLA/Vec3f.h>

/// Noise function based on something like inverse fourier transform (not quite but close).
float noise(float x, float y, float z=0);

/// Add many octaves of noise.
float turbulence(float x, float y, float z=0);

/// Generate a quasi random number according to the Halton sequence of base base.
double HaltonNumber(int index, int base);

inline float smooth_step(float a, float b, float x)
{
    float d = b-a;
    float t = (x-a)/d;
    if(t<0.0) return 0;
    else if(t>1.0) return 1;
    else return t*t*(3 - 2*t);
}

#endif
