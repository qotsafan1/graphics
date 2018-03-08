QT += core gui opengl

CONFIG += c++11

TARGET = IntroScene
TEMPLATE = app

win32 {
    INCLUDEPATH +=  "$${PWD}/glew-1.10.0/include/"
    !contains(QMAKE_TARGET.arch, x86_64) {
        message("x86 build"   )
        LIBS *=  "$${PWD}/glew-1.10.0/lib/Release/Win32/glew32s.lib"
    } else {
        message("x86_64 build")
        LIBS *=  "$${PWD}/glew-1.10.0/lib/Release/x64/glew32s.lib"
    }
}

mac{
    QMAKE_CXX = clang
    eval(QT_MAJOR_VERSION = "4") {
        LIBS += -framework Foundation -framework Cocoa
        OBJECTIVE_SOURCES  +=  lib02564/GLGraphics/core_profile_attributes.mm
    }
}

INCLUDEPATH += lib02564

HEADERS += exercises/Introscene/IntroScene.h \
    lib02564/GLGraphics/ResourceLoader.h
SOURCES += exercises/Introscene/main.cpp exercises/Introscene/IntroScene.cpp \
    lib02564/GLGraphics/ResourceLoader.cpp

HEADERS += lib02564/CGLA/ArithMatFloat.h\
lib02564/CGLA/ArithQuat.h\
lib02564/CGLA/ArithSqMat2x2Float.h\
lib02564/CGLA/ArithSqMat3x3Float.h\
lib02564/CGLA/ArithSqMat4x4Float.h\
lib02564/CGLA/ArithSqMatFloat.h\
lib02564/CGLA/ArithVec.h\
lib02564/CGLA/ArithVec2Float.h\
lib02564/CGLA/ArithVec3Float.h\
lib02564/CGLA/ArithVec3Int.h\
lib02564/CGLA/ArithVec4Float.h\
lib02564/CGLA/ArithVec4Int.h\
lib02564/CGLA/ArithVecFloat.h\
lib02564/CGLA/ArithVecInt.h\
lib02564/CGLA/BitMask.h\
lib02564/CGLA/CGLA.h\
lib02564/CGLA/eigensolution.h\
lib02564/CGLA/ExceptionStandard.h\
lib02564/CGLA/Mat2x2d.h\
lib02564/CGLA/Mat2x2f.h\
lib02564/CGLA/Mat2x3d.h\
lib02564/CGLA/Mat2x3f.h\
lib02564/CGLA/Mat3x3d.h\
lib02564/CGLA/Mat3x3f.h\
lib02564/CGLA/Mat4x4d.h\
lib02564/CGLA/Mat4x4f.h\
lib02564/CGLA/Quatd.h\
lib02564/CGLA/Quaternion.h\
lib02564/CGLA/Quatf.h\
lib02564/CGLA/statistics.h\
lib02564/CGLA/TableTrigonometry.h\
lib02564/CGLA/UnitVector.h\
lib02564/CGLA/Vec2d.h\
lib02564/CGLA/Vec2f.h\
lib02564/CGLA/Vec2i.h\
lib02564/CGLA/Vec2ui.h\
lib02564/CGLA/Vec3d.h\
lib02564/CGLA/Vec3f.h\
lib02564/CGLA/Vec3Hf.h\
lib02564/CGLA/Vec3i.h\
lib02564/CGLA/Vec3uc.h\
lib02564/CGLA/Vec3usi.h\
lib02564/CGLA/Vec4d.h\
lib02564/CGLA/Vec4f.h\
lib02564/CGLA/Vec4i.h\
lib02564/CGLA/Vec4uc.h\
lib02564/GLGraphics/Core3_2_context.h\
lib02564/GLGraphics/GLHeader.h\
lib02564/Mesh/ObjLoader.h\
lib02564/GLGraphics/ShaderProgram.h\
lib02564/GLGraphics/ThreeDObject.h\
lib02564/Mesh/TriangleMesh.h\
lib02564/GLGraphics/User.h\
lib02564/Mesh/Material.h\
lib02564/Mesh/Texmap.h

SOURCES += lib02564/CGLA/ArithSqMat3x3Float.cpp\
lib02564/CGLA/ArithSqMat4x4Float.cpp\
lib02564/CGLA/ArithVec2Float.cpp\
lib02564/CGLA/ArithVec3Float.cpp\
lib02564/CGLA/eigensolution.cpp\
lib02564/CGLA/gel_rand.cpp\
lib02564/CGLA/Mat3x3d.cpp\
lib02564/CGLA/Mat3x3f.cpp\
lib02564/CGLA/Mat4x4d.cpp\
lib02564/CGLA/Mat4x4f.cpp\
lib02564/CGLA/statistics.cpp\
lib02564/CGLA/TableTrigonometry.cpp\
lib02564/CGLA/Vec2i.cpp\
lib02564/CGLA/Vec3f.cpp\
lib02564/CGLA/Vec3i.cpp\
lib02564/GLGraphics/GLHeader.cpp\
lib02564/Mesh/ObjLoader.cpp\
lib02564/GLGraphics/ShaderProgram.cpp\
lib02564/GLGraphics/ThreeDObject.cpp\
lib02564/Mesh/TriangleMesh.cpp\
lib02564/GLGraphics/User.cpp\
lib02564/Mesh/Texmap.cpp

RESOURCES += \
    IntroScene.qrc


OTHER_FILES += \
    shaders/IntroScene/object.frag \
    shaders/IntroScene/object.vert \
    shaders/IntroScene/wire.vert \
    shaders/IntroScene/wire.geom \
    shaders/IntroScene/wire.frag \
    data/cow.obj \
    data/cow.mtl
