#ifndef VOXELWORLD_H
#define VOXELWORLD_H

#include "GLGraphics/User.h"
#include "RGrid.h"
#include <GLGraphics/ShaderProgram.h>
#include <Mesh/TriangleMesh.h>

class VoxelWorld: public GLGraphics::HeightMap
{
    const int WORLD_XDIM = 512;
    const int WORLD_YDIM = 512;
    const int WORLD_ZDIM = 64;
    const int CELL_SIZE = 32;

    RGrid<float> world_grid;

    std::vector<Mesh::TriangleMesh> mesh_grid;

    void build();
    bool is_built = false;

    void mesh_cell(int i, int j, int k);
    void update(const CGLA::Vec3f &_p, int N, std::function<float(const CGLA::Vec3i &)> f);


public:
    VoxelWorld();
    ~VoxelWorld();

    float height(const CGLA::Vec3f&) const;

    void edit(const CGLA::Vec3f&, float amplitude, int n=5);
    void smooth(const CGLA::Vec3f&, float amplitude, int n=5);
    void reinit();
    void smooth();
    /// Draw using OpenGL: Assumes a reasonable shading program is used.
    void draw(GLGraphics::ShaderProgramDraw&);

    CGLA::Vec3f ray_trace(const CGLA::Vec3f &p0, const CGLA::Vec3f &dir) const;
};

#endif // VOXELWORLD_H
