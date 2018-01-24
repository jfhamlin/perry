#ifndef _SOLSTICEGUI_H_
#define _SOLSTICEGUI_H_

#include "SolsticeScene.h"
#include "MainWindow.h"
#include "Math/Matrix.h"
#include <GL/glui.h>

class SolsticeGUI
{
public:

    SolsticeGUI(int x, int y, SceneParameters& params);

    GLUI* GLUIPtr() const { return m_glui; }

    void SetOffsetStep(real_t step);

    void SetMainWindow(const MainWindow* win)
    {
        m_win = win;
        if (m_win)
        {
            m_outputButton->enable();
            m_stlOutputButton->enable();
        }
        else
        {
            m_outputButton->disable();
            m_stlOutputButton->disable();
        }
    }

    void SaveSceneToRIB();
    void OpenFileSelectWindow();

    void SaveSceneToSTL();
    void OpenSTLFileSelectWindow();

    void UpdateUpVector();

private:

    void CreateColorChooser(GLUI_Node* p,
                            const std::string& title,
                            romulus::render::Color& color);

    GLUI_Scrollbar* CreateSlider(GLUI_Node* p,
                                 real_t min, real_t max, real_t step,
                                 const std::string& title, real_t* destValPtr);

    void CreateSlider(GLUI_Node* p,
                      uint_t min, uint_t max, int step,
                      const std::string& title, uint_t* destValPtr);

    SceneParameters& m_params;

    const MainWindow* m_win;

    GLUI* m_glui;

    GLUI_Scrollbar* m_offsetScrollbar;

    GLUI_Button* m_outputButton;
    GLUI_Button* m_stlOutputButton;
    GLUI_EditText* m_outputText;

    romulus::math::Vector3 m_originalUp;
    romulus::math::Matrix<4, 4, float> m_upRotation;
};

#endif // _SOLSTICEGUI_H_
