
//
//  polygonize.cpp
//  PointReconstruction
//
//  Created by J. Andreas Bærentzen on 16/03/13.
//  Copyright (c) 2013 J. Andreas Bærentzen. All rights reserved.
//

#include <unordered_map>
#include "polygonize.h"

using namespace std;
using namespace CGLA;

namespace {
    Vec3f xpf[] = {Vec3f(0,-0.5,-0.5),Vec3f(0,0.5,-0.5),Vec3f(0,0.5,0.5),Vec3f(0,-0.5,0.5)};
    Vec3f xmf[] = {Vec3f(0, 0.5,-0.5),Vec3f(0,-0.5,-0.5),Vec3f(0,-0.5,0.5),Vec3f(0,0.5,0.5)};
    
    Vec3f ypf[] = {Vec3f( 0.5,0, -0.5),Vec3f(-0.5,0, -0.5),Vec3f(-0.5,0, 0.5),Vec3f(0.5,0, 0.5)};
    Vec3f ymf[] = {Vec3f(-0.5,0, -0.5),Vec3f(0.5,0, -0.5),Vec3f(0.5,0, 0.5),Vec3f(-0.5,0, 0.5)};
    
    Vec3f zpf[] = {Vec3f(-0.5,-0.5,0),Vec3f(0.5,-0.5,0),Vec3f(0.5,0.5,0),Vec3f(-0.5,0.5,0)};
    Vec3f zmf[] = {Vec3f( 0.5,-0.5,0),Vec3f(-0.5,-0.5,0),Vec3f(-0.5,0.5,0),Vec3f(0.5,0.5,0)};
}

int polygonize(const RGrid<float>& grid, const CGLA::Vec3i &_corner0, const CGLA::Vec3i &_corner7,
                std::vector<CGLA::Vec3f>& vertices, std::vector<int>& indices, float tau)
{
    auto H = [](const Vec3f& key) {
        Vec3i x(key);
        return size_t(x[0]*73856093)^
                size_t(x[1]*19349663)^
                size_t(x[2]*83492791);
    };
    using VertexIdxMap = unordered_map<Vec3f, int, function<size_t(const Vec3f&)>>;
    VertexIdxMap vertex_idx_map(10000,H);

    auto store_vertex = [&](const Vec3f& p) -> int {
        auto itr = vertex_idx_map.find(p);
        if(itr == vertex_idx_map.end())
        {
            size_t n = vertices.size();
            vertex_idx_map.insert(make_pair(p,(int)n));
            vertices.push_back(p);
            return static_cast<int>(n);
        }
        else return itr->second;
    };

    Vec3i corner0 = v_max(_corner0, Vec3i(0));
    Vec3i corner7 = v_min(_corner7, grid.get_dims());
    vertices.clear();
    for(int i=corner0[0];i<corner7[0];++i)
        for(int j=corner0[1];j<corner7[1];++j)
            for(int k=corner0[2];k<corner7[2];++k)
            {
                Vec3i vox(i,j,k);
                if(grid[vox] <= tau)
                {
                    if(grid[Vec3i(i+1,j,k)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i+0.5, j,k) + xpf[n]));
                    if(grid[Vec3i(i-1,j,k)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i-0.5, j,k) + xmf[n]));
                    if(grid[Vec3i(i,j+1,k)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i, j+0.5,k) + ypf[n]));
                    if(grid[Vec3i(i,j-1,k)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i, j-0.5,k) + ymf[n]));
                    if(grid[Vec3i(i,j,k+1)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i, j,k+0.5) + zpf[n]));
                    if(grid[Vec3i(i,j,k-1)] > tau)
                        for(int n=0;n<4;++n)
                            indices.push_back(store_vertex(Vec3f(i, j,k-0.5) + zmf[n]));
                    
                }
            }
 return static_cast<int>(vertices.size());
}

float interpolate(const RGrid<float>& grid, const Vec3f& _p)
{
    Vec3f p = v_min(Vec3f(grid.get_dims())-Vec3f(1.0f+1e-6f),
                    v_max(_p,Vec3f(0,0,0)));
    Vec3i p0(p);
    Vec3f a[2];
    a[1] = p-Vec3f(p0);
    a[0] = Vec3f(1.0)-a[1];
    float f = 0;
    for(int i=0;i<8;++i) {
            int xbit = i&1;
            int ybit = (i&2)>>1;
            int zbit = (i&4)>>2;
            const Vec3i corner = p0+Vec3i(xbit,ybit,zbit);
            f += a[xbit][0]*a[ybit][1]*a[zbit][2]*grid[corner];
    }
    return f;
}



CGLA::Vec3f grad(const RGrid<float>& grid,const CGLA::Vec3f& p)
{
    Vec3f g(interpolate(grid,p+Vec3f(1,0,0))-interpolate(grid,p-Vec3f(1,0,0)),
            interpolate(grid,p+Vec3f(0,1,0))-interpolate(grid,p-Vec3f(0,1,0)),
            interpolate(grid,p+Vec3f(0,0,1))-interpolate(grid,p-Vec3f(0,0,1)));
    g *= 0.5;
    return g;
}

void push_to_surface(const RGrid<float>& grid,  Vec3f& p, float tau, float max_dist)
{
    for(int iter=0;iter<2;++iter) {
        Vec3f g = grad(grid, p);
        double sl = sqr_length(g);
        double d = (interpolate(grid, p)-tau)/sl;
        Vec3f disp = g*d;
        float disp_len = length(disp);
        p -= disp * min(max_dist/disp_len, 1.0f);
    }
}
