#ifndef __TERRAIN_SCENE_H
#define __TERRAIN_SCENE_H

#include <string>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec4f.h>
#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/User.h>
#include "GBuffer.h"
#include "Terrain.h"
#include "VoxelWorld.h"
#include "Terrain.h"
#include "light.h"
#include "skymodel.h"

const std::string shader_path = "./shaders/TerrainScene/";
const std::string objects_path = "./data/";

class TerrainScene : public QGLWidget
{
    Q_OBJECT

    VoxelWorld terrain;
    //Terrain terrain;

    const DirectionalLight default_light = DirectionalLight(
                CGLA::Vec3f(0.5f,0.5f,1.f),         // direction
                CGLA::Vec4f(0.6f,0.6f,0.3f,0.6f),   // diffuse
                CGLA::Vec4f(0.6f,0.6f,0.3f,0.6f),   // specular
                CGLA::Vec4f(0.3f,0.4f,0.6f,0.4f));  // ambient

    DirectionalLight directional_light = default_light;

    CGLA::Vec3f cursor;
    GLGraphics::User user;

    float edit_amplitude = 0;
    bool reload_shaders = true;
    bool recompute_sky = true;
    enum RenderMode
    {
        DRAW_NORMAL         = 0,
        DRAW_WIRE           = 1,
        DRAW_DEFERRED_TOON  = 2,
        DRAW_SSAO           = 3,
        DRAW_SSAO_GI        = 4
    };
    RenderMode render_mode = DRAW_NORMAL;

    QTimer *timer;

    float ax, ay, dist;
    float ang_x;
    float ang_y;
    int mouse_x0,mouse_y0;
    int window_width;
    int window_height;
    const CGLA::Vec3f map_center = CGLA::Vec3f(256.0f,256.0f,0.0f);

public:
    TerrainScene( const QGLFormat& format, QWidget* parent = 0);

public slots:
    void animate();

protected:
    void set_light_and_camera(GLGraphics::ShaderProgramDraw& shader_prog);
    void render_direct(bool reload);
    void render_direct_wireframe(bool reload);
    void render_to_gbuffer(GBuffer& gbuffer, bool reload);
    void render_deferred_toon(bool reload);
    void render_deferred_ssao(bool reload);
    void render_deferred_ssao_sky(bool reload);
    void get_skybox_textures(bool reload, GLuint &sky, GLuint &sky_integral);
    float get_time_of_day() { return time_of_day; }
    void set_time_of_day(float solar_time);

    void draw_objects(GLGraphics::ShaderProgramDraw& shader_prog, const GLGraphics::HeightMap& terra);
    void draw_trees(GLGraphics::ShaderProgramDraw& shader_prog, const GLGraphics::HeightMap& terra);
    void draw_ocean(GLGraphics::ShaderProgram& shader_prog);
    void draw_screen_aligned_quad(GLGraphics::ShaderProgram& shader_prog);


    void initializeGL();
    void resizeGL( int w, int h );
    void paintGL();
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);


    unsigned int day_of_the_year = 200;
    float time_of_day = 10.0f;
    PreethamSunSky sky_model = PreethamSunSky(
                 day_of_the_year,  // Day of the year in 1,2,...,365
                 time_of_day,      // Time of the day, 24h format
                 45.0f,            // Latitude, in deg
                 2.4f,             // Sky turbidity
                 0.0f);            // Overcast
};


#endif // INTROSCENE_H
