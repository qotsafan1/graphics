#ifndef RGRID_H
#define RGRID_H

#include <vector>
#include <thread>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec3i.h>

/** \brief Regular voxel grid.

        This class template can be instantiated and used directly.
        This is just a regular voxel grid stored as a linear array
        with std::functions to access its contents. */

template<typename T>
class RGrid
{
    using DataType = T;

    CGLA::Vec3i dims;

    /// x size of grid.
    int x_dim;

    /// x size times y size of grid. Stored for efficiency reasons.
    int xy_dim;

    /// Vector containing the actual data.
    std::vector<DataType> data;

public:

    bool in_domain(const CGLA::Vec3i& p) const {
        return p.all_ge(CGLA::Vec3i(0)) && p.all_l(dims);
    }

    CGLA::Vec3i project_onto_domain(const CGLA::Vec3i& _p) const {
        return CGLA::v_max(CGLA::Vec3i(0), CGLA::v_min(dims-CGLA::Vec3i(1),_p));
    }

private:

    /// Convert xyz index into an index in a linear array.
    int grid_idx(const CGLA::Vec3i& _idx) const
    {
        const CGLA::Vec3i idx = project_onto_domain(_idx);
        return  idx[2] * xy_dim + idx[1] * x_dim + idx[0];
    }


    /// The default grid value, used to clear grid.
    DataType default_val;

public:

    /** Construct a regular voxel grid. This std::function
            is passed a Vec3i _dims and an optional
            initialization value, val. It creates a grid
            of specified dimensions, and initializes the
            value of all voxels to val. */
    RGrid(CGLA::Vec3i _dims, const DataType& val = 0):
        dims(_dims),
        x_dim(_dims[0]), xy_dim(_dims[0]*_dims[1]),
        data(_dims[0]*_dims[1]*_dims[2],val),
        default_val(val)
    {}

    /** Store a voxel in a regular grid. */
    void store(const CGLA::Vec3i& p, const DataType& t)
    {
        data[grid_idx(p)] = t;
    }

    /** Read/write access to voxel grid. This is
            a non-const overloaded operator[]. In a regular
            grid, we have reserved room for all voxels in
            advance. Hence, it is possible to create a non-const
            operator[]. See AncestorGrid::operator[]. */
    DataType& operator[](const CGLA::Vec3i& p)
    {
        return data[grid_idx(p)];
    }

    /// Read only access to voxel grid through const operator[]
    const DataType& operator[](const CGLA::Vec3i& p) const
    {
        return data[grid_idx(p)];
    }

    /// Const std::function to get a pointer to the first voxel in grid.
    const DataType* get() const {return &data[0];}

    /// Non-const std::function to get a pointer to the first voxel in grid.
    DataType* get() {return &data[0];}

    /// Get xyz dims
    const CGLA::Vec3i& get_dims() const {return dims;}

    /// Get x dimensions of grid.
    int get_x_dim() const { return x_dim;}

    /// Get x size times y size of grid.
    int get_xy_dim() const { return xy_dim;}

    /// Get length of linear array actually containing voxels.
    int get_size() const { return static_cast<int>(data.size());}

    void clear()
    {
        int N = data.size();
        for(int i=0;i<N;++i)
            data[i] = default_val;
    }


};

const int NO_THREADS = 8;

inline void for_each_voxel_region(RGrid<float>& grid, const CGLA::Vec3i& p0, const CGLA::Vec3i& p7,
                           std::function<void(RGrid<float>&,const CGLA::Vec3i&)> functor)
{
    for(int i=p0[0];i<p7[0];++i)
        for(int j=p0[1];j<p7[1];++j)
            for(int k=p0[2];k<p7[2];++k) {
                CGLA::Vec3i p(i,j,k);
                functor(grid, p);
            }
}

inline void for_each_voxel(RGrid<float>& grid, std::function<void(RGrid<float>&,const CGLA::Vec3i&)> functor)
{
    int no_threads, layers_per_thread;
    int layers = grid.get_dims()[2];
    if(layers >= NO_THREADS) {
        layers_per_thread = layers/NO_THREADS;
        if(layers%NO_THREADS != 0)
            layers_per_thread += 1;
        no_threads = NO_THREADS;
    }
    else
    {
        layers_per_thread = 1;
        no_threads = layers;
    }
    auto f = [&](int k0, int k1) {
        CGLA::Vec3i p0(0);
        CGLA::Vec3i p7 = grid.get_dims();
        p0[2] = k0;
        p7[2] = k1;
        for_each_voxel_region(grid, p0, p7, functor);
    };
    std::vector<std::thread> thread_vec(no_threads);
    for (int i=0;i<no_threads;++i) {
        int k0 = i*layers_per_thread;
        int k1 = std::min(layers,(i+1)*layers_per_thread);
        thread_vec[i] = std::thread(f, k0 , k1);
    }
    for (int i=0;i<no_threads;++i)
        thread_vec[i].join();
}

inline void transform_voxels(RGrid<float>& grid, std::function<float(const RGrid<float>&,const CGLA::Vec3i&)> functor)
{
    auto new_grid = grid;
    auto f = [&](RGrid<float>& g,const CGLA::Vec3i& p) { new_grid[p] = functor(g, p); };
    for_each_voxel(grid,f);
    grid = new_grid;
}


#endif // RGRID_H
