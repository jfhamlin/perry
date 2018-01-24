#include "Render/OpenGL/Utilities.h"
#include "MainWindow.h"
#include "Math/Utilities.h"
#include "RibbedScene.h"
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/scoped_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <GL/glui.h>

using namespace romulus;
using namespace romulus::math;
using namespace romulus::render;
using namespace romulus::render::opengl;

MainWindow::MainWindow(GlutMaster* glutMaster,
                     int setWidth, int setHeight,
                     int setInitPositionX, int setInitPositionY,
                     char* title):
    m_left(false), m_middle(false), m_right(false), m_x(0), m_y(0),
    m_scene(0)
{
    width  = setWidth;
    height = setHeight;

    initPositionX = setInitPositionX;
    initPositionY = setInitPositionY;

    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(width, height);
    glutInitWindowPosition(initPositionX, initPositionY);

    glutMaster->CallGlutCreateWindow(title, this);
    glViewport(0, 0, width, height);

    glEnable(GL_DEPTH_TEST);

    // Set up the camera
    m_camera.SetProjectionTransform(GeneratePerspectiveProjectionTransform(
                                            DegreesToRadians(55.f),
                                            real_t(width) / real_t(height),
                                            0.5f, 150.f));
    m_camera.Dolly(-30.0);
}

MainWindow::~MainWindow()
{
    glutDestroyWindow(windowID);
}

namespace
{
void RenderGCI(const GeometryChunkInstance* gcip)
{
    ASSERT(gcip->GeometryChunk());
    ASSERT(gcip->SurfaceDescription());

    // Push instance's transform.
    PushMultiplyModelViewMatrix pushModelViewMatrix(gcip->Transform());

    // Render geometry chunk.
    const GeometryChunk& gc = *gcip->GeometryChunk();
    ASSERT(gc.IndexCount() % 3 == 0);

    // Set the object-specific shader parameters.
    glColor4fv(gcip->SurfaceDescription()->Color().Data());
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,
                gcip->SurfaceDescription()->SpecularExponent());
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,
                 (gcip->SurfaceDescription()->SpecularAlbedo() *
                  gcip->SurfaceDescription()->Color()).Data());

    // Bind the geometry and render.
    glVertexPointer(3, GL_ROMULUS_REAL, 0, gc.Vertices());
    glNormalPointer(GL_ROMULUS_REAL, 0, gc.Normals());

    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(gc.IndexCount()),
                   GL_UNSIGNED_SHORT, gc.Indices());
}
} // namespace

void MainWindow::CallBackDisplayFunc(void)
{
    ASSERT(m_scene);

    // Update if necessary.
    m_scene->ApplyUpdate();

    // Render the sculpture.
    const romulus::render::Color& bg = m_scene->BackgroundColor();
    glClearColor(bg[0], bg[1], bg[2], 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_SMOOTH);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_DEPTH_TEST);

    {
        PushLoadProjectionMatrix proj(
                m_camera.Camera().ProjectionTransform());
        PushLoadModelViewMatrix view(m_camera.Camera().ViewTransform());

        IScene::GeometryCollection geometry;
        IScene::LightCollection lights;
        m_scene->PotentiallyVisibleGeometry(geometry,
                                            m_camera.Camera().ViewFrustum());
        m_scene->PotentiallyRelevantLights(lights,
                                           m_camera.Camera().ViewFrustum());

        // Set lighting parameters.
        glEnable(GL_LIGHTING);
        glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
        glEnable(GL_COLOR_MATERIAL);

        IScene::LightCollection::iterator it = lights.begin();
        for (uint_t i = 0; it != lights.end() && i < 8; ++i, ++it)
        {
            glEnable(GL_LIGHT0 + i);
            Vector4 pos((*it)->Position(), 1.0);
            romulus::render::Color col = (*it)->Intensity() * (*it)->Color();
            glLightfv(GL_LIGHT0 + i, GL_POSITION, pos.Data());
            glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, col.Data());
            glLightfv(GL_LIGHT0 + i, GL_SPECULAR, col.Data());
        }

        // Enable the client state once.
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);


        std::for_each(geometry.begin(), geometry.end(),
                      boost::bind(&RenderGCI, _1));

        // Disable client state.
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);

        for (uint_t i = 0; i < lights.size() && i < 8; ++i)
            glDisable(GL_LIGHT0 + i);

        glDisable(GL_COLOR_MATERIAL);
        glDisable(GL_LIGHTING);
    }

    glutSwapBuffers();
    //GLUI_Master.sync_live_all();
}

// Top left is (0, 0), bottom right is (width, height).
void MainWindow::CallBackReshapeFunc(int w, int h){

    width = w;
    height= h;

    glViewport(0, 0, width, height);
    m_camera.SetProjectionTransform(GeneratePerspectiveProjectionTransform(
                                            DegreesToRadians(55.f),
                                            real_t(width) / real_t(height),
                                            0.5f, 150.f));

    CallBackDisplayFunc();
}

void MainWindow::CallBackMouseFunc(int button, int state, int x, int y)
{
    switch (button)
    {
        case GLUT_LEFT_BUTTON:
            m_left = (state == GLUT_DOWN);
            break;
        case GLUT_MIDDLE_BUTTON:
            m_middle = (state == GLUT_DOWN);
            break;
        case GLUT_RIGHT_BUTTON:
            m_right = (state == GLUT_DOWN);
            break;
    }

    m_x = x;
    m_y = y;
}

void MainWindow::CallBackMotionFunc(int x, int y)
{
    real_t relX = x - m_x;
    real_t relY = y - m_y;

    m_x = x;
    m_y = y;

    const real_t Sensitivity = 0.025;

    if (m_left && m_right)
    {
        m_camera.Dolly(Sensitivity * -relY);
    }
    else if (m_left)
    {
        Vector2 dir(-relX, relY);
        real_t mag = Sensitivity * Magnitude(dir) * 0.075 * Pi;

        m_camera.Pan(dir, mag);
    }
    else if (m_right)
    {
        Vector2 dir(Sensitivity * -relX,
                    Sensitivity * relY);
        m_camera.Track(dir);
    }

    CallBackDisplayFunc();
}

void MainWindow::CallBackKeyboardFunc(unsigned char key, int x, int y)
{
    Vector3 disp(0, 0, 0);
    switch (key)
    {
        default:
            break;
    }
}

void MainWindow::SetScene(RibbedScene* scene)
{
    if (scene != m_scene)
    {
        m_scene = scene;
    }
}
