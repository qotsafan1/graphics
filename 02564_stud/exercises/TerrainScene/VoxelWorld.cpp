#include <thread>
#include <fstream>
#include "VoxelWorld.h"
#include "polygonize.h"
#include "auxil.h"

#define SOLUTION_CODE 1


using namespace std;
using namespace CGLA;
using namespace Mesh;
using namespace GLGraphics;

VoxelWorld::VoxelWorld(): world_grid(Vec3i(WORLD_XDIM, WORLD_YDIM, WORLD_ZDIM), 0),
    mesh_grid(WORLD_XDIM*WORLD_YDIM*WORLD_ZDIM/(CELL_SIZE*CELL_SIZE*CELL_SIZE)){}

VoxelWorld::~VoxelWorld() {
    cout << "Saving the world" << endl;
    ofstream ofs("world.raw", ios::binary);
    ofs.write(reinterpret_cast<char*>(world_grid.get()),sizeof(float)*world_grid.get_size());
}


void VoxelWorld::mesh_cell(int i, int j, int k)
{
    Vec3i c0 = Vec3i(i,j,k)*CELL_SIZE;
    Vec3i c7 = Vec3i(i+1, j+1, k+1)*CELL_SIZE;
    int mesh_idx = i + (WORLD_XDIM/CELL_SIZE)*j + (WORLD_XDIM*WORLD_YDIM/(CELL_SIZE*CELL_SIZE))*k;

    vector<Vec3f> positions;
    vector<int> indices;
    if(polygonize(world_grid, c0, c7, positions, indices, 0)>0) {

        vector<Vec3f> grads;
        for(auto& p: positions) {
            Vec3f g;
            push_to_surface(world_grid,p,0,1);
            grads.push_back(grad(world_grid,p));
        }
        mesh_grid[mesh_idx] = Mesh::TriangleMesh();
        mesh_grid[mesh_idx].add("vertex", positions);
        mesh_grid[mesh_idx].add("normal", grads);
        vector<GLuint> elementIndices;
        for (size_t i=0;i<indices.size();i+=4){
            if(sqr_length(positions[indices[i]]-positions[indices[i+2]])<
                    sqr_length(positions[indices[i+1]]-positions[indices[i+3]])) {
                elementIndices.push_back(indices[i]);
                elementIndices.push_back(indices[i+1]);
                elementIndices.push_back(indices[i+2]);
                elementIndices.push_back(indices[i]);
                elementIndices.push_back(indices[i+2]);
                elementIndices.push_back(indices[i+3]);
            } else {
                elementIndices.push_back(indices[i]);
                elementIndices.push_back(indices[i+1]);
                elementIndices.push_back(indices[i+3]);
                elementIndices.push_back(indices[i+1]);
                elementIndices.push_back(indices[i+2]);
                elementIndices.push_back(indices[i+3]);
            }
        }
        static Material mat;
        mesh_grid[mesh_idx].add_draw_call(elementIndices, mat, GL_TRIANGLES);
        mesh_grid[mesh_idx].build_vertex_array_object();
    }
}

void VoxelWorld::update(const CGLA::Vec3f& _p, int N, function<float(const CGLA::Vec3i&)> f)
{
    Vec3i p(_p + Vec3f(0.5));
    Vec3i p0 = world_grid.project_onto_domain(p-Vec3i(N));
    Vec3i p7 = world_grid.project_onto_domain(p+Vec3i(N)) + Vec3i(1);

    RGrid<float> new_grid(p7-p0);
    for_each_voxel_region(new_grid,p0,p7,[&](RGrid<float>& g,const CGLA::Vec3i& off) {
        float t = smooth_step(N, N-3, length(Vec3f(off-p)));
        //float t = exp(-sqr_length(off-p)/(0.1*N*N));
        g[off-p0] = world_grid[off] * (1.0-t) + t *f(off);
    });
    for_each_voxel_region(world_grid,p0,p7,[&](RGrid<float>& g,const CGLA::Vec3i& off) {
        g[off] = new_grid[off-p0];
    });
    Vec3i c0 = v_max(Vec3i(0), (p0-Vec3i(1)) / CELL_SIZE);
    Vec3i c7 = v_min(Vec3i(WORLD_XDIM/CELL_SIZE-1, WORLD_YDIM/CELL_SIZE-1, WORLD_ZDIM/CELL_SIZE-1),
                     (p7) / CELL_SIZE);
    for(int i=c0[0];i<=c7[0];++i)
        for(int j=c0[1];j<=c7[1]; ++j)
            for(int k=c0[2];k<=c7[2]; ++k)
                mesh_cell(i,j,k);
}


void VoxelWorld::build()
{
    float bumps = 0; //Period
    float b_height = 16000.0;
    float cave_height = 24; //Size of noise
    float s2 = 32.0; //Many small or big ones

    float c = 400.0;

    cout << "Calculating Voxels" << endl;
    Vec2i v(256,256);

    int x0 = v[0];
    int y0 = v[1];

    // Create a function that takes a grid g as argument and a voxel
    auto f = [&](RGrid<float>& g, const CGLA::Vec3i& p) {
        // 2D center

        float x = p[0];
        float y = p[1];
        float z = p[2];

        float height = turbulence(x/25,y/25,z/25)*noise(x/12,y/12,z/2)*b_height*(sin(x/440)*cos(y/435)) + 4.0;
        float volume = z - height;

        float clamped = min(c, max((-c),volume));
        float porous = clamped + cave_height * abs(noise(s2*x,s2*y,s2*z));

        // assign a value to the grid at the given voxel
        g[p] = porous;

    };

    // Now call this for each voxel … parallelized
    for_each_voxel(world_grid, f);

    cout << "polygonizing world" << endl;

    for(int i=0;i<WORLD_XDIM/CELL_SIZE;++i)
        for(int j=0;j<WORLD_YDIM/CELL_SIZE; ++j)
            for(int k=0;k<WORLD_ZDIM/CELL_SIZE; ++k)
                mesh_cell(i,j,k);
}

float VoxelWorld::height(const Vec3f & p) const
{
    float x = p[0];
    float y = p[1];
    float current_z = p[2];
    if(interpolate(world_grid,Vec3f(x,y,current_z))<0)
        while(interpolate(world_grid,Vec3f(x,y,current_z))<0 && current_z < WORLD_ZDIM) current_z += 0.2f;
    while(interpolate(world_grid,Vec3f(x,y,current_z))>0 && current_z > 0) current_z -= 0.1f;

    return current_z;
}

float avg_neighbors(const RGrid<float>& grid, const CGLA::Vec3i& p) {
    float avg_val = 0;
    int cnt = 0;
    for(int ii=-1;ii<=1;++ii)
        for(int jj=-1;jj<=1;++jj)
            for(int kk=-1;kk<=1;++kk) {
                Vec3i off_n = grid.project_onto_domain(p + Vec3i(ii,jj,kk));
                avg_val += grid[off_n];
                ++cnt;
            }
    return avg_val / cnt;
}

void VoxelWorld::smooth()
{
    transform_voxels(world_grid, avg_neighbors);
}

void VoxelWorld::edit(const Vec3f & p, float amplitude, int N)
{
    update(p, N, [&](const CGLA::Vec3i&){return amplitude;});
}


void VoxelWorld::smooth(const Vec3f & p, float amplitude, int N)
{
    auto f = [&](const CGLA::Vec3i& p) -> float {
        return amplitude * avg_neighbors(world_grid, p);
    };
    update(p, N, f);
}

float reinit_voxel(const RGrid<float>& grid, const CGLA::Vec3i& p) {
    float v0 = grid[p];
    auto grad_axis = [&](const Vec3i& a) {
        float pp = grid[p + a] - v0;
        float pm = v0 - grid[p - a];
        if(v0>0)
            return max(max(pm,0.0f),-min(pp,0.0f));
        else
            return max(-min(pm,0.0f), max(pp, 0.0f));
    };

    Vec3f g(grad_axis(Vec3i(1,0,0)),
            grad_axis(Vec3i(0,1,0)),
            grad_axis(Vec3i(0,0,1)));

    auto s = [](float x){return x/(sqrt(x*x+1));};
    v0 -= 0.5*s(v0)*(length(g)-1.0);
    return v0;
};


void VoxelWorld::reinit()
{
    transform_voxels(world_grid, reinit_voxel);
}



CGLA::Vec3f VoxelWorld::ray_trace(const CGLA::Vec3f& p0, const CGLA::Vec3f& dir) const {
    Vec3f p = p0;
    const float step = 0.1f;
    while(interpolate(world_grid,p)>0 && world_grid.in_domain(Vec3i(p))) p += step*dir;
    return p;
}

/// Draw using OpenGL: Assumes a reasonable shading program is used.
void VoxelWorld::draw(GLGraphics::ShaderProgramDraw& shader)
{
    static GLuint noise_tex = 0;

    if(!is_built) {
        build();
        int DIM = 256;
        glGenTextures(1, &noise_tex);
        glBindTexture(GL_TEXTURE_3D, noise_tex);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        vector<float> noise_vec(DIM*DIM*DIM);
        gel_srand(0);
        for(int i=0;i<DIM*DIM*DIM;++i)
            noise_vec[i] =  gel_rand()/static_cast<float>(GEL_RAND_MAX);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, DIM, DIM,DIM, 0, GL_RED, GL_FLOAT, &noise_vec[0]);
        glGenerateMipmap(GL_TEXTURE_3D);
        is_built = true;
    }
    shader.set_model_matrix(identity_Mat4x4f());
    shader.use_texture(GL_TEXTURE_3D, "noise_tex", noise_tex,1);
    for(auto& m : mesh_grid)
        m.render(shader);
}
