/*
 *  aux.cpp
 *  02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include <GLGraphics/GLHeader.h>
#include "auxil.h"
#include <CGLA/Vec2f.h>
#include <CGLA/Vec3f.h>
#include <cmath>
#include <vector>
#include <future>

using namespace std;
using namespace CGLA;


double HaltonNumber(int index, int base)
{
	double result = 0.0;
	double f = 1.0 / base;
	int i = index;
	while(i>0)
	{
		result += f * (i%base);
		i = floor(i/double(base));
		f = f/base;
	}
	return result;
}

float noise(float x, float y, float z)
{
    static const int NSINES = 100;
    static vector<Vec3f> freqs(NSINES);
	static bool was_here = false;
	if(!was_here)
	{
        gel_srand(0);
        was_here = true;
		for(int i=0;i<NSINES;++i)
		{
            Vec3f v(0);
			float l;
			do {
                v = 2.0f*Vec3f(gel_rand(), gel_rand(), gel_rand())/((float)GEL_RAND_MAX)-Vec3f(1);
				l = length(v);
			}
            while(l<0.5f || l>1.0f);
			freqs[i] = v;
		}
	}
	float h=0;
	for(int i=0;i<NSINES;++i)
        h += sin(dot(Vec3f(x,y,z), freqs[i]));
	return (1.0/NSINES)*h;
}

float turbulence(float x, float y, float z)
{
        float scale=1.0;
        float t=0;
        for(int i=0;i<4;++i,scale *= 2)
            t += noise(x*scale, y*scale, z*scale)/scale;
        return t;
}
