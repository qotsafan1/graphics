/*
 *  main.cpp - Terrain Scene 02564_Framework
 *
 *  Created by J. Andreas Bærentzen on 01/02/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#define SOLUTION_CODE

#include <iostream>

#include <QApplication>
#include <QTimer>
#include <QTime>
#include <QDebug>
#include <QMouseEvent>
#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Vec2f.h>
#include <GLGraphics/GLHeader.h>
#include <GLGraphics/ShaderProgram.h>
#include <GLGraphics/ThreeDObject.h>
#include <GLGraphics/User.h>

#include "ShadowBuffer.h"
#include "auxil.h"
#include "lsys.h"
#include "Terrain.h"
#include "VoxelWorld.h"
#include "TerrainScene.h"

#include "cubemapbuffer.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

void TerrainScene::draw_screen_aligned_quad(ShaderProgram& shader_prog)
{
    const Vec2f points[] = {Vec2f(-1,-1), Vec2f(1,-1), Vec2f(-1,1), Vec2f(1,1)};
    GLuint pos_attrib = shader_prog.get_attrib_location("vertex");
    static GLuint VAO = 0;
    if(VAO == 0)
    {
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,4*sizeof(Vec2f),&points[0],GL_STATIC_DRAW);
        glVertexAttribPointer(pos_attrib,2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(pos_attrib);
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}

void TerrainScene::draw_ocean(ShaderProgram& shader_prog)
{

    GLuint pos_attrib = shader_prog.get_attrib_location("vertex");
    static GLuint VAO = 0;
    if(VAO == 0)
    {
        const Vec3f points[] = {Vec3f(-1024,-1024,0) + map_center, Vec3f(1024,-1024,0) + map_center,
                                Vec3f(-1024,1024,0) + map_center, Vec3f(1024,1024,0) + map_center};
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER,4*sizeof(Vec3f),&points[0],GL_STATIC_DRAW);
        glVertexAttribPointer(pos_attrib,3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(pos_attrib);
    }
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLE_STRIP,0,4);
}


void TerrainScene::draw_objects(ShaderProgramDraw& shader_prog, const GLGraphics::HeightMap&)
{
    auto f = [&](const Vec2f &map_center_offset, float height_offset)
    {
        Vec3f plane_pos_abs = map_center + Vec3f(map_center_offset[0],map_center_offset[1],64.0f);
        return Vec3f(plane_pos_abs[0], plane_pos_abs[1], terrain.height(plane_pos_abs) + height_offset);
    };

    static vector<ThreeDObject> objects;
    
    if(objects.empty())
    {
        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"cottage_obj/cottage.obj");
        objects.back().scale(Vec3f(1));
        objects.back().translate(f(Vec2f(-16.0f, +14.0f), 0.0f));

        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"cow.obj");
        objects.back().scale(Vec3f(.5));
        objects.back().rotate(Vec3f(1,0,0), 90);
        objects.back().translate(f(Vec2f(-6.0f, -14.0f), 1.6f));

        objects.push_back(ThreeDObject());
        objects[objects.size()-1].init(objects_path+"portal.obj");
        objects.back().scale(Vec3f(2));
        objects.back().translate(f(Vec2f(-20.0f, 0.0f), 1.0f));
    }
    for(int i=0;i<objects.size();++i)
        objects[i].display(shader_prog);
}
/*
void TerrainScene::draw_trees(ShaderProgramDraw& shader_prog, const GLGraphics::HeightMap& terra)
{
    static GLint count;
    static GLuint vao = make_tree(count);
    Vec3f trans(200,200,200);
    trans[2] = terra.height(trans);
    shader_prog.set_model_matrix(translation_Mat4x4f(trans) *scaling_Mat4x4f(Vec3f(0.010f)));
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES,0, count);
    glBindVertexArray(0);
}
*/

void TerrainScene::draw_trees(ShaderProgramDraw& shader_prog, const GLGraphics::HeightMap& terra)
{
    const int N = 100;

    static GLint count;
    static GLuint vao = make_tree(count);
    static Mat4x4f M[N];
    static Vec4f mat_diff[N];
    static bool was_here = false;

    if(!was_here) {
        was_here = true;

        for(int i=0;i<N;++i) {
            Vec3f p(198.0 + (rand() / float(RAND_MAX))*100.0, 198.0 + rand() / (float(RAND_MAX))*100.0,rand() / (float(RAND_MAX))*100); // Set p to some random position.

            p[2] = terra.height(p);
            M[i] = transpose(translation_Mat4x4f(p)*scaling_Mat4x4f(Vec3f(max(0.01, (rand() / float(RAND_MAX))*0.05))));

            // Multiply a random scaling and a rotation on to M[i]
            mat_diff[i] = Vec4f(rand() / (float(RAND_MAX)),rand() / (float(RAND_MAX)), rand() / (float(RAND_MAX)),0); // make this guy random too.
        }
    }
    shader_prog.set_model_matrix(identity_Mat4x4f());
    glBindVertexArray(vao);

    for(int i=0;i<100;++i) {
        glUniformMatrix4fv(shader_prog.get_uniform_location("InstanceMatrix"), 1, GL_FALSE,(const GLfloat*) &M[i]);
        glUniform4fv(shader_prog.get_uniform_location("mat_diff"),1, (const GLfloat*) &mat_diff[i]);
        glDrawArrays(GL_TRIANGLE_STRIP,0, count);
    }
}

void TerrainScene::set_light_and_camera(ShaderProgramDraw& shader_prog)
{
    shader_prog.set_projection_matrix(perspective_Mat4x4f(55.0f, float(window_width)/window_height, 0.4f, 800));
    shader_prog.set_view_matrix(user.get_view_matrix());
    directional_light.set_in_shader(shader_prog);
}


void TerrainScene::render_direct(bool reload)
{
    // Load shaders for terrain and general objects. The terrain color is computed
    // procedurally, so we need a different shader.
    static ShaderProgramDraw terrain_shader(shader_path, "terrain.vert", "", "terrain.frag");
    static ShaderProgramDraw object_shader(shader_path, "object.vert", "", "object.frag");
    static ShaderProgramDraw ocean_shader(shader_path, "ocean.vert", "", "ocean.frag");
    static ShaderProgramDraw tree_shader(shader_path, "tree.vert", "", "tree.frag");

    if(reload)
    {
        directional_light = default_light;
        ocean_shader.reload();
        terrain_shader.reload();
        object_shader.reload();
        tree_shader.reload();
    }

    glClearColor(0.4f,0.35f,0.95f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    terrain_shader.use();
    set_light_and_camera(terrain_shader);
    terrain.draw(terrain_shader);

    object_shader.use();
    set_light_and_camera(object_shader);
    draw_objects(object_shader, terrain);

    tree_shader.use();
    set_light_and_camera(tree_shader);
    draw_trees(tree_shader, terrain);


    ocean_shader.use();
    set_light_and_camera(ocean_shader);
    draw_ocean(ocean_shader);
}

void TerrainScene::render_direct_wireframe(bool reload)
{
    // Create shader program. Note that this is the only program which has
    // a geometry shader. Also there is only one shader in this function since
    // all geometry is treated in the same way.
    static ShaderProgramDraw wire_shader(shader_path, "wire.vert", "wire.geom", "wire.frag");
    if(reload)
        wire_shader.reload();

    glClearColor(0.9f,0.9f,0.9f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    wire_shader.use();
    set_light_and_camera(wire_shader);
    Vec2f win_scale = Vec2f(window_width/2.0, window_height/2.0);
    wire_shader.set_uniform("WIN_SCALE", win_scale);

    terrain.draw(wire_shader);
    draw_objects(wire_shader, terrain);
    draw_trees(wire_shader, terrain);
}


void TerrainScene::render_to_gbuffer(GBuffer& gbuffer, bool reload)
{
    static ShaderProgramDraw objects_render_to_gbuffer(shader_path, "objects_gbuffer.vert", "", "objects_gbuffer.frag");
    static ShaderProgramDraw terrain_render_to_gbuffer(shader_path, "terrain_gbuffer.vert", "", "terrain_gbuffer.frag");

    if(reload)
    {
        objects_render_to_gbuffer.reload();
        terrain_render_to_gbuffer.reload();
    }
    gbuffer.enable();

    terrain_render_to_gbuffer.use();
    set_light_and_camera(terrain_render_to_gbuffer);

    terrain.draw(terrain_render_to_gbuffer);

    objects_render_to_gbuffer.use();
    set_light_and_camera(objects_render_to_gbuffer);
    draw_objects(objects_render_to_gbuffer, terrain);

    draw_trees(objects_render_to_gbuffer, terrain);
}

void TerrainScene::render_deferred_toon(bool reload)
{
    // Initialize resources. Note that we have a G-Buffer here. This buffer captures eye
    // space position, normal, and color for each pixel. We then do deferred shading based
    // on that.
    static GBuffer gbuffer(window_width, window_height);

    static ShaderProgramDraw deferred_shading(shader_path, "deferred.vert", "", "deferred_toon.frag");

    gbuffer.rebuild_on_resize(window_width, window_height);
    if(reload)
    {
        directional_light = default_light;
        deferred_shading.reload();
    }

    render_to_gbuffer(gbuffer, reload);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    glDisable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);
    deferred_shading.use();
    set_light_and_camera(deferred_shading);

    gbuffer.bind_textures(0, 1, 2);
    deferred_shading.set_uniform("gtex", 0);
    deferred_shading.set_uniform("ntex", 1);
    deferred_shading.set_uniform("ctex", 2);
    draw_screen_aligned_quad(deferred_shading);
    glEnable(GL_DEPTH_TEST);
}

void TerrainScene::render_deferred_ssao(bool reload)
{
    const int NO_DISC_POINTS = 32;
    const int SHADOW_SIZE = 4096;
    
    // Create all resources.
    static GLuint  ssao_tex, fbo;
    static vector<Vec3f> disc_points;
    static GBuffer gbuffer(window_width, window_height);
    gbuffer.rebuild_on_resize(window_width, window_height);
    static ShaderProgramDraw deferred_ssao(shader_path, "deferred.vert", "", "deferred_ssao.frag");
    static ShaderProgramDraw deferred_combination(shader_path, "deferred.vert", "", "deferred_ssao_combination.frag");
    static ShaderProgramDraw render_to_shadow_map(shader_path, "shadow.vert", "", "shadow.frag");
    static ShadowBuffer shadow_buffer(SHADOW_SIZE);
    const int MMW=window_width/2;
    const int MMH=window_height/2;

    // Reinitialize all shaders and buffers if reload = true
    if(reload)
    {
        directional_light = default_light;
        gbuffer.initialize(window_width, window_height);
        deferred_ssao.reload();
        deferred_combination.reload();
        render_to_shadow_map.reload();
        shadow_buffer.initialize();
    }
    
    // If reload=true or we are here first time, reinitialize the FBO for SSAO computation.
    static bool was_here = false;
    if(reload || !was_here)
    {
        was_here = true;

        glGenTextures(1, &ssao_tex);
        glBindTexture(GL_TEXTURE_RECTANGLE, ssao_tex);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, MMW, MMH, 0, GL_RGBA, GL_FLOAT, 0);
        
        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, ssao_tex, 0);
        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Something wrong with FBO" << endl;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Also: Create a bunch of vectors for hemisphere sampling.
        gel_srand(0);
        disc_points.clear();
        for(int i=0;i<NO_DISC_POINTS;++i)
        {
            float alpha = 2.0 * M_PI * i / float(NO_DISC_POINTS) - M_PI;
            Vec3f v(cos(alpha),sin(alpha),0);
            v*= gel_rand()/float(GEL_RAND_MAX);
            disc_points.push_back(v);
        }
    }

    // Enable shadow buffer and the program for drawing to shadow buffer
    shadow_buffer.enable();
    render_to_shadow_map.use();
    
    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f v = Vec3f(directional_light.direction);
    render_to_shadow_map.set_view_matrix(lookat_Mat4x4f(map_center,-v,Vec3f(0,0,1)));
    render_to_shadow_map.set_model_matrix(identity_Mat4x4f());
    render_to_shadow_map.set_projection_matrix(ortho_Mat4x4f(Vec3f(-300,-300,-100),Vec3f(300,300,400)));

    // Switch viewport size to that of shadow buffer.
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glClearColor(0,1,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    // Draw to shadow buffer.
    terrain.draw(render_to_shadow_map);
    draw_objects(render_to_shadow_map, terrain);
    draw_trees(render_to_shadow_map, terrain);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // We need to reset the viewport, since the shadow buffer does not have
    // the same size as the screen window.
    glViewport(0, 0, window_width, window_height);

    // Enable GBuffer and render to GBuffer
    render_to_gbuffer(gbuffer, reload);
    // Entering deferred rendering, so disable depth test
    glDisable(GL_DEPTH_TEST);
    
    // Bind the FBO that we render to in the first pass where we compute the SSAO contribution
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // We need to change the viewport since we render SSAO to smaller buffer.
    glViewport(0, 0, MMW, MMH);
    glClear(GL_COLOR_BUFFER_BIT);
    deferred_ssao.use();

    // Bind the GBuffer, assign uniforms including the hemisphere direction vectors.
    gbuffer.bind_textures(0, 1, 2);
    deferred_ssao.set_uniform("gtex", 0);
    deferred_ssao.set_uniform("ntex", 1);
    deferred_ssao.set_uniform("ctex", 2);
    glUniform3fv(deferred_ssao.get_uniform_location("disc_points"),NO_DISC_POINTS,
                 reinterpret_cast<float*>(&disc_points[0]));
    deferred_ssao.set_uniform("NO_DISC_POINTS",NO_DISC_POINTS);
    Vec2f windowDimension(window_width, window_height);
    deferred_ssao.set_uniform("win_dim", windowDimension);

    //Bind shadow buffer texture
    shadow_buffer.bind_textures(3);
    deferred_ssao.set_uniform("shadow_map", 3);

    // Tricky stuff: Create an inverse of the modelview matrix to (later) be able
    // to go from the GBuffer coordinates (eye space) to world coordinates. We multiply
    // this inverse onto the matrix created above in order to go directly from eye space
    // to shadow buffer space.
    Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
    mat *= scaling_Mat4x4f(Vec3f(0.5));
    mat *= render_to_shadow_map.get_projection_matrix();
    mat *= render_to_shadow_map.get_view_matrix();
    mat *= invert(user.get_view_matrix());
    deferred_ssao.set_uniform("Mat", mat);

    // Draw quad to render SSAO computation
    draw_screen_aligned_quad(deferred_ssao);

    // Bind regular on-screen framebuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    // Bind program for combining SSAO and other shading.
    deferred_combination.use();
    set_light_and_camera(deferred_combination);

    // Bind texture resources, and assign uniforms
    gbuffer.bind_textures(0, 1, 2);
    deferred_combination.set_uniform("gtex", 0);
    deferred_combination.set_uniform("ntex", 1);
    deferred_combination.set_uniform("ctex", 2);
    
    deferred_combination.use_texture(GL_TEXTURE_RECTANGLE, "ssao_tex", ssao_tex, 3);

    shadow_buffer.bind_textures(4);
    deferred_combination.set_uniform("shadow_map", 4);
    deferred_combination.set_uniform("Mat", mat);
    
    // Go back to right viewport size
    glViewport(0, 0, window_width, window_height);
    
    // Draw quad in order to run the program that combines
    // SSAO and shading.
    draw_screen_aligned_quad(deferred_combination);

    // Reenable depth test
    glEnable(GL_DEPTH_TEST);
}

void TerrainScene::render_deferred_ssao_sky(bool reload)
{
    const int NO_DISC_POINTS = 32;
    const int SHADOW_SIZE = 4096;

    glClearColor(0.4f,0.35f,0.95f,0.0f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Create all resources.
    static GLuint  ssao_tex, fbo;
    static vector<Vec3f> disc_points;
    static GBuffer gbuffer(window_width, window_height);
    gbuffer.rebuild_on_resize(window_width, window_height);
    static ShaderProgramDraw deferred_ssao(shader_path, "deferred.vert", "", "deferred_ssao.frag");
    static ShaderProgramDraw deferred_combination(shader_path, "deferred.vert", "", "deferred_ssao_combination_sky.frag");
    static ShaderProgramDraw render_to_shadow_map(shader_path, "shadow.vert", "", "shadow.frag");

    static ShadowBuffer shadow_buffer(SHADOW_SIZE);
    const int MMW=window_width/2;
    const int MMH=window_height/2;

    // Reinitialize all shaders and buffers if reload = true
    if(reload || recompute_sky)
        directional_light = sky_model.get_directional_light(0.8f);
    if(reload)
    {
        gbuffer.initialize(window_width, window_height);
        deferred_ssao.reload();
        deferred_combination.reload();
        render_to_shadow_map.reload();
        shadow_buffer.initialize();
    }

    // If reload=true or we are here first time, reinitialize the FBO for SSAO computation.
    static bool was_here = false;
    if(reload || !was_here)
    {
        was_here = true;

        glGenTextures(1, &ssao_tex);
        glBindTexture(GL_TEXTURE_RECTANGLE, ssao_tex);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, MMW, MMH, 0, GL_RGBA, GL_FLOAT, 0);

        glGenFramebuffers(1,&fbo);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, ssao_tex, 0);
        if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            cout << "Something wrong with FBO" << endl;
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        // Also: Create a bunch of vectors for hemisphere sampling.
        gel_srand(0);
        disc_points.clear();
        for(int i=0;i<NO_DISC_POINTS;++i)
        {
            float alpha = 2.0 * M_PI * i / float(NO_DISC_POINTS) - M_PI;
            Vec3f v(cos(alpha),sin(alpha),0);
            v*= gel_rand()/float(GEL_RAND_MAX);
            disc_points.push_back(v);
        }
    }

    // Enable shadow buffer and the program for drawing to shadow buffer
    shadow_buffer.enable();
    render_to_shadow_map.use();

    // Set up a modelview matrix suitable for shadow: Maps from world coords to
    // shadow buffer coords.
    Vec3f up = (directional_light.direction[2] == 0.0f)? Vec3f(0,1,0) : Vec3f(0,0,1); // To avoid gimbal lock
    Vec3f v = Vec3f(directional_light.direction);
    render_to_shadow_map.set_view_matrix(lookat_Mat4x4f(map_center,-v,up));
    render_to_shadow_map.set_model_matrix(identity_Mat4x4f());
    render_to_shadow_map.set_projection_matrix(ortho_Mat4x4f(Vec3f(-300,-300,-100),Vec3f(300,300,400)));

    // Switch viewport size to that of shadow buffer.
    glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
    glClearColor(0,1,0,0);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Draw to shadow buffer.
    terrain.draw(render_to_shadow_map);
    draw_objects(render_to_shadow_map, terrain);
    draw_trees(render_to_shadow_map, terrain);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glDrawBuffer(GL_BACK);

    // We need to reset the viewport, since the shadow buffer does not have
    // the same size as the screen window.
    glViewport(0, 0, window_width, window_height);

    static ShaderProgramDraw ocean_render_to_gbuffer(shader_path, "ocean_gbuffer.vert", "", "ocean_gbuffer.frag");

    // Enable GBuffer and render to GBuffer
    render_to_gbuffer(gbuffer, reload);

    // Adding rendering of ocean in the gbuffer

    ocean_render_to_gbuffer.use();
    set_light_and_camera(ocean_render_to_gbuffer);
    draw_ocean(ocean_render_to_gbuffer);

    // Entering deferred rendering, so disable depth test
    glDisable(GL_DEPTH_TEST);

    // Bind the FBO that we render to in the first pass where we compute the SSAO contribution
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);

    // We need to change the viewport since we render SSAO to smaller buffer.
    glViewport(0, 0, MMW, MMH);
    glClear(GL_COLOR_BUFFER_BIT);
    deferred_ssao.use();

    // Bind the GBuffer, assign uniforms including the hemisphere direction vectors.
    gbuffer.bind_textures(0, 1, 2);
    deferred_ssao.set_uniform("gtex", 0);
    deferred_ssao.set_uniform("ntex", 1);
    deferred_ssao.set_uniform("ctex", 2);
    glUniform3fv(deferred_ssao.get_uniform_location("disc_points"),NO_DISC_POINTS,
                 reinterpret_cast<float*>(&disc_points[0]));
    deferred_ssao.set_uniform("NO_DISC_POINTS",NO_DISC_POINTS);
    Vec2f windowDimension(window_width, window_height);
    deferred_ssao.set_uniform("win_dim", windowDimension);

    //Bind shadow buffer texture
    shadow_buffer.bind_textures(3);
    deferred_ssao.set_uniform("shadow_map", 3);

    // Tricky stuff: Create an inverse of the modelview matrix to (later) be able
    // to go from the GBuffer coordinates (eye space) to world coordinates. We multiply
    // this inverse onto the matrix created above in order to go directly from eye space
    // to shadow buffer space.
    Mat4x4f mat = translation_Mat4x4f(Vec3f(0.5));
    mat *= scaling_Mat4x4f(Vec3f(0.5));
    mat *= render_to_shadow_map.get_projection_matrix();
    mat *= render_to_shadow_map.get_view_matrix();
    mat *= invert(user.get_view_matrix());
    deferred_ssao.set_uniform("Mat", mat);

    // Draw quad to render SSAO computation
    draw_screen_aligned_quad(deferred_ssao);

     // Bind regular on-screen framebuffer.
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    glViewport(0, 0, window_width, window_height);
    // Draw quad in order to run the program that combines
    // SSAO and shading.

    GLuint sky_irradiance, sky_texture;
    get_skybox_textures(reload, sky_texture, sky_irradiance);

    // Bind program for combining SSAO and other shading.
    deferred_combination.use();
    set_light_and_camera(deferred_combination);

    // Bind texture resources, and assign uniforms
    gbuffer.bind_textures(0, 1, 2);
    deferred_combination.set_uniform("gtex", 0);
    deferred_combination.set_uniform("ntex", 1);
    deferred_combination.set_uniform("ctex", 2);

    deferred_combination.use_texture(GL_TEXTURE_RECTANGLE, "ssao_tex", ssao_tex, 3);

    shadow_buffer.bind_textures(4);
    deferred_combination.set_uniform("shadow_map", 4);

    deferred_combination.use_texture(GL_TEXTURE_CUBE_MAP, "sky_irradiance", sky_irradiance, 5);
    deferred_combination.use_texture(GL_TEXTURE_CUBE_MAP, "sky_texture", sky_texture, 6);

    deferred_combination.set_uniform("Mat", mat);

    Mat4x4f inv_V = invert(user.get_view_matrix());
    deferred_combination.set_uniform("ss_view_to_world", inv_V);
    Mat4x4f inv_V_3x3(Vec4f(Vec3f(inv_V[0]), 0.0f), Vec4f(Vec3f(inv_V[1]), 0.0f), Vec4f(Vec3f(inv_V[2]), 0.0f), Vec4f(0.0f));
    deferred_combination.set_uniform("ss_proj_view_inv", inv_V_3x3*invert(deferred_combination.get_projection_matrix()));
    deferred_combination.set_uniform("win_scale", Vec2f(1.0f/window_width, 1.0/window_height));
    draw_screen_aligned_quad(deferred_combination);

    // Reenable depth test
    glEnable(GL_DEPTH_TEST);
}

void TerrainScene::get_skybox_textures(bool reload, GLuint& sky, GLuint& sky_integral)
{
    const int SKY_RESOLUTION = 512;
    const int N = 2000;
    const int no_of_iters = N/5;
    static GLuint cubemap = 0;
    static GLuint cubemap_integrated = 0;
    static CubemapBuffer buff1(SKY_RESOLUTION);
    static CubemapBuffer buff2(SKY_RESOLUTION);
    static ShaderProgramDraw sky_compute(shader_path, "integrator.vert","integrator.geom","integrator.frag");
    static int iteration = 0;

    if(recompute_sky)
    {
        sky_model.init();
        sky_model.precomputeTexture(cubemap, SKY_RESOLUTION);
        recompute_sky = false;
        iteration = 0;
        if(reload)
            sky_compute.reload();
        cout << flush << "Computing integrated skymap" << flush;
    }
    if(iteration < no_of_iters)
    {
        sky_compute.use();
        glViewport(0,0,SKY_RESOLUTION,SKY_RESOLUTION);
        CubemapBuffer& buff_new = iteration%2 ? buff2 : buff1;
        CubemapBuffer& buff_old = iteration%2 ? buff1 : buff2;
        buff_new.enable_write();
        sky_compute.use_texture(GL_TEXTURE_CUBE_MAP, "sky_texture", cubemap, 0);
        sky_compute.use_texture(GL_TEXTURE_CUBE_MAP, "old_texture", buff_old.get_texture(), 1);
        sky_compute.set_uniform("inv_cubemap_size", Vec2f(1.0f/SKY_RESOLUTION));
        sky_compute.set_uniform("N", N/no_of_iters);
        sky_compute.set_uniform("iteration", iteration);
        draw_screen_aligned_quad(sky_compute);
        glFinish();
        cubemap_integrated = buff_new.get_texture();
        ++iteration;
        if(iteration == no_of_iters)
            cout << "Done." << endl << flush;
        else if(iteration%40 == 0)
            cout << ".";

        // Resetting values
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glViewport(0, 0, window_width, window_height);
    }
    sky = cubemap;
    sky_integral = cubemap_integrated;
}

void TerrainScene::set_time_of_day(float solar_time)
{
    while(solar_time >= 24.0f)
    {
      solar_time -= 24.0f;
      if(--day_of_the_year < 1)
          day_of_the_year = 365;
    }
    while(solar_time < 0.0f)
    {
      solar_time += 24.0f;
      if(++day_of_the_year > 365)
          day_of_the_year = 1;
    }
    time_of_day = solar_time;
    sky_model.setTime(day_of_the_year, time_of_day);
    recompute_sky = true;
}

TerrainScene::TerrainScene( const QGLFormat& format, QWidget* parent)
    : QGLWidget( new Core3_2_context(format), (QWidget*) parent),
      ax(0), ay(0), dist(12),ang_x(0),ang_y(0),
      user(&terrain),mouse_x0(0),mouse_y0(0)
{
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(animate()));
    timer->start(16);
}

void TerrainScene::paintGL()
{


    switch(render_mode)
    {
    case DRAW_NORMAL:
        render_direct(reload_shaders);
        break;
    case DRAW_WIRE:
        render_direct_wireframe(reload_shaders);
        break;
    case DRAW_DEFERRED_TOON:
        render_deferred_toon(reload_shaders);
        break;
    case DRAW_SSAO:
        render_deferred_ssao(reload_shaders);
        break;
    case DRAW_SSAO_GI:
        render_deferred_ssao_sky(reload_shaders);
        break;
    }
    reload_shaders = false;
    check_gl_error();



}

void TerrainScene::resizeGL(int W, int H)
{
    window_width = W;
    window_height = H;
    glViewport(0, 0, window_width, window_height);
    reload_shaders = true;
}


void TerrainScene::animate()
{
    user.update_position();
    repaint();
}

void TerrainScene::initializeGL()
{
    setup_gl();
    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);
}

void TerrainScene::mousePressEvent(QMouseEvent *m)
{
    mouse_x0 = m->x();
    mouse_y0 = m->y();
}

void TerrainScene::mouseReleaseEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/window_width;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/window_height;
    ang_x += delta_ang_x;
    ang_y += delta_ang_y;
}

void TerrainScene::mouseMoveEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/window_width;
    float delta_ang_y = M_PI*(mouse_y0-m->y())/window_height;
    float ax = ang_x + delta_ang_x;
    float ay = ang_y + delta_ang_y;
    Vec3f dir(cos(ax)*cos(ay),sin(ax)*cos(ay),sin(ay));
    user.set_dir(dir);
    QWidget::repaint();
}

void TerrainScene::keyPressEvent(QKeyEvent *e)
{
    static bool is_full_screen = false;
    switch(e->key())
    {
    case Qt::Key_Escape: QApplication::quit();
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
        render_mode = static_cast<RenderMode>(e->key()-'1');
        reload_shaders = true;
        break;
    case ' ':
        render_mode = static_cast<RenderMode>((static_cast<int>(render_mode)+1)%5);
        reload_shaders = true;
        break;
    case 'W': user.forward();  break;
    case 'S': user.back();break;
    case 'A': user.strafe_left();break;
    case 'D': user.strafe_right();break;
    case 'X': user.toggle_fly_mode(); break;
    case 'F':
        if(is_full_screen) {
            showNormal();
            is_full_screen = false;
        }
        else
        {
            showMaximized();
            is_full_screen = true;
        }
        break;
    case 'R':
        reload_shaders = true;
        break;
    case 'L':
        {
            Vec3f v = normalize(-user.get_dir() + Vec3f(0,0,1));
            directional_light.direction = v;
        }
        break;
    case 'O':
        user.set(Vec3f(-7.60765f, 14.907f, 4.37377f), Vec3f(0.333226f, -0.925571f, 0.179661f));
        break;
    case 'H':
        {
            float solar_time = get_time_of_day();
            solar_time += e->modifiers() & Qt::ShiftModifier ? -0.5f : 0.5f;
            set_time_of_day(solar_time);
            cout << "The solar time is now: " << solar_time << endl;
        }
        break;
    default:
        QWidget::keyPressEvent(e);
        break;
    }
    QWidget::repaint();
}

void TerrainScene::keyReleaseEvent(QKeyEvent *)
{
    user.stop();
}

int main(int argc, char *argv[])
{
    const int WINDOW_WIDTH = 1136;
    const int WINDOW_HEIGHT = 640;

    //qDebug() << argv[0] << " - " << argv[1] << endl;
    QApplication a( argc, argv );

    QGLFormat glFormat;
    glFormat.setVersion( 3, 2 );
    glFormat.setProfile( QGLFormat::CoreProfile ); // Requires >=Qt-4.8.0
    glFormat.setDepthBufferSize(32);
    TerrainScene w( glFormat);
    w.resize(WINDOW_WIDTH, WINDOW_HEIGHT);
    w.setFocusPolicy(Qt::ClickFocus);
    w.show();

    return a.exec();
}
