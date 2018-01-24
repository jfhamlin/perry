#include "SolsticeGUI.h"
#include "Utility/SceneToRIB.h"
#include "Utility/SceneToSTL.h"
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>

using namespace romulus;
using namespace romulus::math;

extern SolsticeGUI* g_GUI;

namespace
{
void UpdateCallback(int)
{
    //GLUI_Master.sync_live_all();
}

void IdleFunc()
{
    GLUI_Master.sync_live_all();
}
} // namespace

void OpenFileSelectWindow(int)
{
    g_GUI->OpenFileSelectWindow();
}

void OpenSTLFileSelectWindow(int)
{
    g_GUI->OpenSTLFileSelectWindow();
}

void UpdateUpVector(int)
{
    g_GUI->UpdateUpVector();
}


void SaveSceneToRIB(int)
{
    g_GUI->SaveSceneToRIB();
}

SolsticeGUI::SolsticeGUI(int x, int y, SceneParameters& params):
    m_params(params), m_win(0), m_glui(0),
    m_offsetScrollbar(0), m_outputButton(0),
    m_originalUp(params.UpVector)
{
    m_glui = GLUI_Master.create_glui("Solstice CP", 0, x, y);

    // Sculpture parameter rollout
    GLUI_Rollout* sr = new GLUI_Rollout(
        m_glui, "Sculpture Parameters", true);
    // Create the sliders for revolutions.
    CreateSlider(sr, 1u, 10u, 1u, "P Revolutions",
                 &params.PRev);
    CreateSlider(sr, 1u, 10u, 1u, "Q Revolutions",
                 &params.QRev);

    // Create the sliders for the radii.
    CreateSlider(sr, 0.0, 20.0, 0.1, "P Radius",
                 &params.PRad);
    CreateSlider(sr, 0.0, 20.0, 0.1, "Q Radius",
                 &params.QRad);

    // Create a slider for the number of struts.
    CreateSlider(sr, 10u, 1000u, 50, "Num. of Ribs",
                 &params.NumStruts);

    // Create a slider for the strut offset.
    m_offsetScrollbar = CreateSlider(sr, -720.0, 720.0, 0.5,
                                     "Rib P Offset",
                                     &params.StrutOffset);

    // Create a slider for the strut bend.
    CreateSlider(sr, -3.0, 3.0, 0.05, "Rib Bend",
                 &params.StrutBend);

    // Create the sliders for the element thicknesses.
    CreateSlider(sr, 0.1, 0.4, 0.05, "Rail Thickness",
                 &params.RailThickness);
    CreateSlider(sr, 0.05, 0.4, 0.05, "Rib Thickness",
                 &params.StrutThickness);

    // Geometry resolution rollout.
    GLUI_Rollout* gr = new GLUI_Rollout(
        m_glui, "Geometry Resolution", false);
    CreateSlider(gr, 3, 40, 1, "Rib Len. Sections",
                 &params.StrutLengthSections);
    CreateSlider(gr, 6, 15, 1, "Rib Rad. Sections",
                 &params.StrutRadiusSections);

    // Color control rollout.
    GLUI_Rollout* cr = new GLUI_Rollout(
        m_glui, "Colors", false);
    // Create color control sliders.
    CreateColorChooser(cr, "Background Color",
                       params.BackgroundColor);
    CreateColorChooser(cr, "Sculpture Color",
                       params.SculptureColor);

    // Output rollout.
    GLUI_Rollout* op = new GLUI_Rollout(
        m_glui, "RIB/STL Output", false);

    // Ground plane checkbox.
    new GLUI_Checkbox(op, "Show ground plane",
                      &params.RenderGround);

    m_outputText = new GLUI_EditText(op, "Filename:");
    m_outputText->set_text("./solstice.rib");
    m_outputButton = new GLUI_Button(op, "Save to RIB...",
                                     0, &::OpenFileSelectWindow);
    m_outputButton->disable();

    m_stlOutputButton = new GLUI_Button(op, "Save to STL...",
                                     0, &::OpenSTLFileSelectWindow);
    m_stlOutputButton->disable();


    // Up vector (for ground plane placement).
    SetIdentity(m_upRotation);
    //new GLUI_Rotation(op, "Up Vector", &m_upRotation[0][0],
    //                  -1, &::UpdateUpVector);

    GLUI_Master.set_glutIdleFunc(IdleFunc);
}

void SolsticeGUI::SetOffsetStep(real_t step)
{
    m_offsetScrollbar->set_speed(step);
}

void SolsticeGUI::UpdateUpVector()
{
    m_params.UpVector = Submatrix<3, 3, 0, 0>(m_upRotation) * m_originalUp;
}

void SolsticeGUI::CreateColorChooser(GLUI_Node* p,
                                     const std::string& title,
                                     romulus::render::Color& color)
{
    GLUI_Panel* colorPanel = new GLUI_Panel(p, title.c_str());
    const char* names[] = {"Red", "Green", "Blue"};

    for (int i = 0; i < 3; ++i)
    {
        CreateSlider(colorPanel, 0.0, 1.0, 0.05, names[i], &color[i]);
    }
}

GLUI_Scrollbar* SolsticeGUI::CreateSlider(GLUI_Node* p,
                                          real_t min, real_t max, real_t step,
                                          const std::string& title,
                                          real_t* destValPtr)
{
    GLUI_Panel* pan = new GLUI_Panel(p, 0);
    GLUI_EditText* et = new GLUI_EditText(pan, title.c_str(), destValPtr,
                                            -1, &UpdateCallback);
    new GLUI_Column(pan, false);
    GLUI_Scrollbar* sb = new GLUI_Scrollbar(pan, title.c_str(),
                                            GLUI_SCROLL_HORIZONTAL,
                                            destValPtr, -1, &UpdateCallback);

    sb->set_speed(step);
    sb->velocity_limit = 5. * step;
    sb->set_float_limits(min, max);
    et->set_float_limits(min, max);

    return sb;
}

void SolsticeGUI::CreateSlider(GLUI_Node* p,
                               uint_t min, uint_t max, int step,
                               const std::string& title, uint_t* destValPtr)
{
    GLUI_Panel* pan = new GLUI_Panel(p, 0);
    GLUI_EditText* et = new GLUI_EditText(pan, title.c_str(),
                                          reinterpret_cast<int*>(destValPtr),
                                          -1, &UpdateCallback);
    new GLUI_Column(pan, false);
    GLUI_Scrollbar* sb = new GLUI_Scrollbar(pan, title.c_str(),
                                            GLUI_SCROLL_HORIZONTAL,
                                            reinterpret_cast<int*>(destValPtr),
                                            -1, &UpdateCallback);
    sb->set_speed(step);
    sb->velocity_limit = 5 * step;
    sb->set_int_limits(min, max);
    et->set_int_limits(min, max);
}

void SolsticeGUI::OpenFileSelectWindow()
{
    SaveSceneToRIB();
}

void SolsticeGUI::SaveSceneToRIB()
{
    ASSERT(m_win);
    std::ofstream out(m_outputText->get_text());
    if (!out.good())
    {
        std::cerr << "Could not open file '" << m_outputText->get_text() <<
                "' for writing." << std::endl;
        return;
    }
    SceneFrameToRIB(*m_win->Scene(), m_win->Camera(),
                    m_win->Width(), m_win->Height(),
                    "romulus.tiff", out);
    out.close();
}

void SolsticeGUI::OpenSTLFileSelectWindow()
{
    SaveSceneToSTL();
}

void SolsticeGUI::SaveSceneToSTL()
{
    ASSERT(m_win);
    std::ofstream out(m_outputText->get_text());
    if (!out.good())
    {
        std::cerr << "Could not open file '" << m_outputText->get_text() <<
                "' for writing." << std::endl;
        return;
    }
    SceneFrameToSTL(*m_win->Scene(), out);
    out.close();
}
