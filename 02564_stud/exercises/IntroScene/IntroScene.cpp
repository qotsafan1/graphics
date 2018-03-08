/*
 * IntroScene: This is a simple program which allows you to explore and understand the 02564 framework with less
 * pain than if you had a bigger code base.
 *
 *  User interaction:
 *     Mouse rotates,
 *     '+-' zooms in and out,
 *     [ESC] quits,
 *     'r' reloads shaders.
 *     'w' switches to wire.
 *
 */

#include <GLGraphics/GLHeader.h>
#include <iostream>
#include <string>
#include <QApplication>
#include <QtGui>
#include <QString>

#include <CGLA/Mat4x4f.h>
#include <CGLA/Vec3f.h>
#include <CGLA/Quatf.h>
#include "IntroScene.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

const string shader_path = "./shaders/IntroScene/";
const string objects_path = "./data/";

IntroScene::IntroScene( const QGLFormat& format, QWidget* parent)
    : QGLWidget( new Core3_2_context(format), (QWidget*) parent),
      dist(12),ang_x(0),ang_y(0), use_wire(false) {}

void IntroScene::load_shaders()
{
    // Load the shaders and set material parameters.
    shader.init(shader_path, "object.vert", "", "object.frag");
    shader.use();
    shader.set_material(Vec4f(.8,.8,.8,1),Vec4f(.2,.2,.2,1),1);
    shader_wire.init(shader_path, "wire.vert", "wire.geom", "wire.frag");
}

void IntroScene::initializeGL()
{
    setup_gl(); // Mostly to get the right OpenGL version and extensions loaded

    load_shaders();

    // Load 3D model
    cow.init(objects_path+"cow.obj");

    glClearColor( 0.7f, 0.7f, 0.7f, 0.0f );
    glEnable(GL_DEPTH_TEST);
}

void IntroScene::set_light_and_camera(ShaderProgramDraw& shader_prog)
{
    static const Vec4f light_specular(0.6,0.6,0.3,0.6);
    static const Vec4f light_diffuse(0.6,0.6,0.3,0.6);
    static const Vec4f light_ambient(0.3,0.4,0.6,0.4);
    
    Mat4x4f V =
            translation_Mat4x4f(Vec3f(0,0,- dist))
            *rotation_Mat4x4f(XAXIS, ay)
            *rotation_Mat4x4f(YAXIS, ax);
    shader_prog.use();
    shader_prog.set_projection_matrix(perspective_Mat4x4f(55, float(geometry().width())/geometry().height(), 1, 100));
    shader_prog.set_light_position(Vec4f(0,0,1,0));
    shader_prog.set_view_matrix(V);
    shader_prog.set_light_intensities(light_diffuse, light_specular, light_ambient);
}

void IntroScene::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    ShaderProgramDraw& s =  use_wire ? shader_wire : shader;

    s.use();
    set_light_and_camera(s);
    cow.display(s);
    check_gl_error();
}

void IntroScene::resizeGL( int w, int h )
{
    glViewport( 0, 0, w, h );
}

void IntroScene::mousePressEvent(QMouseEvent *m)
{
    mouse_x0 = m->x();
    mouse_y0 = m->y();
}

void IntroScene::mouseReleaseEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/geometry().width();
    float delta_ang_y = M_PI*(mouse_y0-m->y())/geometry().height();
    ang_x += delta_ang_x;
    ang_y += delta_ang_y;
}

void IntroScene::mouseMoveEvent(QMouseEvent *m)
{
    float delta_ang_x = 4*M_PI*(mouse_x0-m->x())/geometry().width();
    float delta_ang_y = M_PI*(mouse_y0-m->y())/geometry().height();
    ax = ang_x + delta_ang_x;
    ay = ang_y + delta_ang_y;
    QWidget::repaint();
}

void IntroScene::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Escape: exit(0);
    case '+': dist /= 1.1; break;
    case '-': dist *= 1.1; break;
    case 'W': use_wire = ! use_wire; break;
    case 'R': load_shaders(); break;
    default:
        QWidget::keyPressEvent(e);
    }
    QWidget::repaint();
}

