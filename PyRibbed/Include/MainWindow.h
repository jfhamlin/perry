#ifndef _MAINWINDOW_H_
#define _MAINWINDOW_H_

#include "glutMaster.h"
#include "Utility/TargetCamera.h"

class RibbedScene;

class MainWindow : public GlutWindow
{
public:

    MainWindow(GlutMaster* glutMaster,
              int setWidth, int setHeight,
              int setInitPositionX, int setInitPositionY,
              char* title);
    virtual ~MainWindow();

    virtual void CallBackDisplayFunc(void);
    virtual void CallBackReshapeFunc(int w, int h);

    virtual void CallBackMouseFunc(int button, int state, int x, int y);
    virtual void CallBackMotionFunc(int x, int y);
    virtual void CallBackKeyboardFunc(unsigned char key, int x, int y);

    int Width() const { return width; }
    int Height() const { return height; }

    void SetScene(RibbedScene* scene);
    const RibbedScene* Scene() const { return m_scene; }
    const romulus::render::Camera& Camera() const
    { return m_camera.Camera(); }

private:

    int height, width;
    int initPositionX, initPositionY;
    romulus::TargetCamera m_camera;

    // Input state.
    bool m_left, m_middle, m_right;
    int m_x, m_y;

    // Ribbed scene.
    RibbedScene* m_scene;
};

#endif // _MAINWINDOW_H_
