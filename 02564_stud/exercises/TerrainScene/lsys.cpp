//
//  L-Systems example (D0L Systems as described in Prusinkiewicz et al.
//  "L-systems: from the Theory to Visual Models of Plants"
//  Created by J. Andreas Bærentzen on 08/02/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <stack>
#include <cmath>
#include <CGLA/Vec3f.h>
#include <CGLA/Mat4x4f.h>
#include <GLGraphics/ShaderProgram.h>

#define SOLUTION_CODE

#include "lsys.h"

using namespace std;
using namespace CGLA;
using namespace GLGraphics;

// Produce the mesh for a truncated cone

// Produce the mesh for a truncated cone
void truncated_cone(const Mat4x4f& m,           // transformation matrix used for the points
                    float l,                    // length of truncated cone
                    float w0,                   // width at base
                    float w1,                   // width at top
                    vector<Vec3f>& triangles,   // triangles (out)
                    vector<Vec3f>& normals)     // normals (out)
{
    float len = sqrt(l*l + sqr(w0-w1));
    float a = l/len;
    float b = (w0-w1)/len;
    const int N = 10;
    for(int i=0;i<=N;++i)
    {
        float alpha = 2.0*M_PI*float(i)/N;
        Vec3f p0 = m.mul_3D_point(Vec3f(w0*cos(alpha), w0*sin(alpha), 0));
        Vec3f p1 = m.mul_3D_point(Vec3f(w1*cos(alpha), w1*sin(alpha), l));
        Vec3f n0 = m.mul_3D_vector(a*Vec3f(cos(alpha), sin(alpha), b));
        Vec3f n1 = n0;

        alpha = 2.0*M_PI*float(i+1)/N;
        Vec3f p2 = m.mul_3D_point(Vec3f(w0*cos(alpha), w0*sin(alpha), 0));
        Vec3f p3 = m.mul_3D_point(Vec3f(w1*cos(alpha), w1*sin(alpha), l));
        Vec3f n2 = m.mul_3D_vector(a*Vec3f(cos(alpha), sin(alpha), b));
        Vec3f n3 = n2;

        normals.push_back(n0);
        triangles.push_back(p0);

        normals.push_back(n1);
        triangles.push_back(p1);

        normals.push_back(n3);
        triangles.push_back(p3);

        normals.push_back(n3);
        triangles.push_back(p3);

        normals.push_back(n2);
        triangles.push_back(p2);

        normals.push_back(n0);
        triangles.push_back(p0);
    }
}

// Symbols used for L-Systems
enum LSSymbol { LS_TURN = '+', LS_ROLL = '/', LS_WIDTH = '!', LS_A = 'A',
                LS_LEFT_BRACKET = '[', LS_RIGHT_BRACKET = ']', LS_DRAW = 'F'};

// LSElement is an LSSymbol and associated parameters.
struct LSElement
{
    LSSymbol symbol;
    double datum1, datum2;
    int data;

    LSElement(LSSymbol _symbol): symbol(_symbol), data(0) {}
    LSElement(LSSymbol _symbol, double _datum1): symbol(_symbol), datum1(_datum1), data(1) {}
    LSElement(LSSymbol _symbol, double _datum1, double _datum2): symbol(_symbol), datum1(_datum1), datum2(_datum2), data(2) {}

    void print(ostream& os)
    {
        os << static_cast<char>(symbol);
        if(data>0)
        {
            os << "(" << datum1;
            if(data>1)
                os << "," << datum2;
            os << ")";
        }
    }
};

// This class represents a single rule with some parameters that can be tuned to
// create specific structures. See Fig. 8 and Table 1 in Prusinkiewicz's paper.
class Rule
{
    float alpha1, alpha2, phi1, phi2, r1, r2, q, e, smin;

public:
    Rule(float _alpha1, float _alpha2, float _phi1, float _phi2,
         float _r1, float _r2, float _q, float _e, float _smin):
        alpha1(_alpha1), alpha2(_alpha2), phi1(_phi1), phi2(_phi2),
        r1(_r1), r2(_r2), q(_q), e(_e), smin(_smin) {}

    bool apply(const LSElement elem, vector<LSElement>& out)
    {
        if(elem.symbol == LS_A)
        {
            float s = elem.datum1;
            float w = elem.datum2;

                if(s>=smin)
                {
                    //Working tree rule
                    out.push_back(LSElement(LS_WIDTH, w));
                    out.push_back(LSElement(LS_DRAW, s));
                    out.push_back(LSElement(LS_LEFT_BRACKET));
                    out.push_back(LSElement(LS_TURN, alpha1));
                    out.push_back(LSElement(LS_ROLL, phi1));
                    out.push_back(LSElement(LS_A, s*r1, w*pow(q,e)));
                    out.push_back(LSElement(LS_RIGHT_BRACKET));
                    out.push_back(LSElement(LS_LEFT_BRACKET));
                    out.push_back(LSElement(LS_TURN, alpha2));
                    out.push_back(LSElement(LS_ROLL, phi2));
                    out.push_back(LSElement(LS_A, s*r2, w*pow(1-q,e)));
                    out.push_back(LSElement(LS_RIGHT_BRACKET));

                    return true;
                }
        }
        else out.push_back(elem);

        return false;
    }
};

// The state used for turtle graphics
struct TurtleState
{
    float w;
    Mat4x4f M;
    float alpha;
    float phi;
    TurtleState(float w0, const Mat4x4f& M0): w(w0), M(M0), alpha(0.0), phi(0.0) {}
};

// The turtle. Contains state and functions for crawling in 3D.
class Turtle
{
    TurtleState turtle_state;
    stack<TurtleState> tss;

public:
    Turtle(float w0): turtle_state(w0, identity_Mat4x4f()) {}
    void turn(float angle) {
        float rad_angle = M_PI/180.0*angle;
        turtle_state.M = turtle_state.M*rotation_Mat4x4f(YAXIS, -rad_angle);
        turtle_state.alpha += rad_angle;
    }
    void roll(float angle) {
        float rad_angle = M_PI/180.0*angle;
        turtle_state.M = turtle_state.M*rotation_Mat4x4f(ZAXIS, -rad_angle);
        turtle_state.phi += rad_angle;
    }
    void move(float dist) {
        turtle_state.M = turtle_state.M*translation_Mat4x4f(dist*Vec3f(0,0,1));
    }
    void push() {
        tss.push(turtle_state);
    }
    void pop() {
        turtle_state = tss.top();
        tss.pop();
    }
    void set_width(float w) {turtle_state.w = w;}
    float get_width() const {return turtle_state.w;}
    const Mat4x4f& get_transform() const {return turtle_state.M;}
};

// This function uses the turtle graphics approach to generate a skeletal representation of the tree.
void interpret(vector<LSElement> str, float w0,
               vector<Vec3f>& triangles, vector<Vec3f>& normals)
{
    Turtle turtle(w0);
    for(size_t i=0;i<str.size(); ++i)
    {
        LSElement elem = str[i];
        switch(elem.symbol)
        {
            case LS_DRAW:
                truncated_cone(turtle.get_transform(), elem.datum1,
                               turtle.get_width(), turtle.get_width()*0.75,
                               triangles, normals);
                turtle.move(elem.datum1);
                break;

            case LS_WIDTH:
                turtle.set_width(elem.datum1);
                break;

            case LS_LEFT_BRACKET:
                turtle.push();
                break;

            case LS_RIGHT_BRACKET:
                turtle.pop();
                break;

            case LS_TURN:
                turtle.turn(elem.datum1);
                break;

            case LS_ROLL:
                turtle.roll(elem.datum1);
                break;

            case LS_A:
                turtle.set_width(elem.datum2);
                truncated_cone(turtle.get_transform(), elem.datum1,
                               turtle.get_width(), turtle.get_width()*0.75,
                               triangles, normals);
                turtle.move(elem.datum1);
                break;

        }
    }
}

// Create an OpenGL vertex array from a vector of triangles and normals
GLuint create_vertex_array_object(vector<Vec3f>& triangles, vector<Vec3f>& normals)
{
    GLuint VAO, VBO[2];
    GLuint vert_attrib = ShaderProgramDraw::get_generic_attrib_location("vertex");
    GLuint norm_attrib = ShaderProgramDraw::get_generic_attrib_location("normal");
    GLuint texcoord_attrib = ShaderProgramDraw::get_generic_attrib_location("texcoord");
    glGenVertexArrays(1, &VAO);
    glGenBuffers(2, &VBO[0]);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER,triangles.size()*sizeof(Vec3f),&triangles[0],GL_STATIC_DRAW);
    glVertexAttribPointer(vert_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER,normals.size()*sizeof(Vec3f),&normals[0],GL_STATIC_DRAW);
    glVertexAttribPointer(norm_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttrib3f(texcoord_attrib, 0, 0,0);
    glEnableVertexAttribArray(vert_attrib);
    glEnableVertexAttribArray(norm_attrib);
    return VAO;
}

// make_tree ties it all together producing a vertex array object with the geometry of a tree
// model created using L-Systems.
GLuint make_tree (GLint& count)
{
    float w0 = 20;                  // Initial branch width
    vector<LSElement> str;           // Initial string
    str.push_back(LSElement(LS_A, 100, w0)); // Push the initial symbol

    // Rules for a coarse example tree
    Rule ex(40,-40,90,-90,0.5,0.5,0.5,0.5,0.5);

    // Now apply the rule a number of times.
    for(int iter=0;iter<8;++iter)
    {
        vector<LSElement> out_str;
        for(size_t e=0;e<str.size(); ++e)
            ex.apply(str[e], out_str);
        str = out_str;
    }

    // Interpret the string producing a vector of triangles and normals
    vector<Vec3f> triangles, normals;
    interpret(str, w0, triangles, normals);

    // Generate a vertex array object and return that.
    count = static_cast<GLint>(triangles.size());
    return create_vertex_array_object(triangles, normals);
}
