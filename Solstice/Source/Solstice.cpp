#include "glutMaster.h"
#include "glutWindow.h"
#include "MainWindow.h"
#include "SolsticeGUI.h"
#include "SolsticeScene.h"

using namespace romulus;
using namespace render;
using namespace math;
using namespace platform;

GlutMaster* g_GlutMaster;
MainWindow* g_MainWindow;
SolsticeGUI* g_GUI;

int main(int argc, char** argv)
{
    // Create the glut master.
    g_GlutMaster = new GlutMaster();

    // Initialize the scene and GUI.
    SceneParameters params;
    g_GUI = new SolsticeGUI(100, 300, params);
    SolsticeScene scene(g_GUI, params);

    // Create the display window.
    std::string winName = "Solstice";
    g_MainWindow = new MainWindow(g_GlutMaster,
                                  800, 800,
                                  610, 50,
                                  const_cast<char*>(winName.c_str()));
    g_MainWindow->SetScene(&scene);
    g_GUI->SetMainWindow(g_MainWindow);

    g_GUI->GLUIPtr()->set_main_gfx_window(g_MainWindow->GetWindowID());
    g_GlutMaster->CallGlutMainLoop();

    return 0;
}
