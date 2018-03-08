//
//  polygonize.h
//  PointReconstruction
//
//  Created by J. Andreas Bærentzen on 16/03/13.
//  Copyright (c) 2013 J. Andreas Bærentzen. All rights reserved.
//

#ifndef __polygonize__
#define __polygonize__

#include <iostream>

#include "RGrid.h"

float interpolate(const RGrid<float>& grid, const CGLA::Vec3f& _p);
int polygonize(const RGrid<float>& grid, const CGLA::Vec3i& _corner0, const CGLA::Vec3i& _corner7,
                std::vector<CGLA::Vec3f>& vertices, std::vector<int>& indices, float tau);
CGLA::Vec3f grad(const RGrid<float>& grid,const CGLA::Vec3f& p);
void push_to_surface(const RGrid<float>& grid, CGLA::Vec3f& _p, float tau, float max_dist);

#endif /* defined(__PointReconstruction__polygonize__) */

