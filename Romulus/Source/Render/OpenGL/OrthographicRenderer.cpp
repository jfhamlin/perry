#include "Render/OpenGL/GLInterface.h"
#include "Render/OpenGL/OrthographicRenderer.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"
#include "Render/OpenGL/GLee.h"
#include "Math/Matrix.h"
#include "Math/Vector.h"
#include "Math/Utilities.h"
#include <GL/glu.h>

namespace romulus
{
using namespace math;

namespace render
{
namespace opengl
{
OrthographicRenderer::OrthographicRenderer(const GLInterface& gli):
    m_textureMgr(gli.TextureMgr)
{
    ASSERT(m_textureMgr);
}

OrthographicRenderer::~OrthographicRenderer()
{
}

void OrthographicRenderer::RenderRectangle(const Rectangle& rect,
                                           const Color& diffuse)
{
    PushAttribute attr(GL_CURRENT_BIT);
    PushLoadModelViewMatrix modelview;

    glColor4f(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
    const Vector2& upperLeft = rect.Origin();
    const Vector2 lowerRight = upperLeft + rect.Extents();

    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(upperLeft[0], upperLeft[1], 0);
    glVertex3f(upperLeft[0], lowerRight[1], 0);
    glVertex3f(lowerRight[0], upperLeft[1], 0);
    glVertex3f(lowerRight[0], lowerRight[1], 0);
    glEnd();
}

void OrthographicRenderer::RenderRectangle(const Rectangle& rect,
                                  const TextureResourceHandleBase& texture)
{
    PushAttribute attr(GL_CURRENT_BIT);
    PushLoadModelViewMatrix modelview;

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    m_textureMgr->BindTexture(0, texture);
    m_textureMgr->SetUnit(0, true);

    const Vector2& upperLeft = rect.Origin();
    const Vector2 lowerRight = upperLeft + rect.Extents();

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(0, 1); glVertex3f(upperLeft[0], upperLeft[1], 0);
    glTexCoord2f(0, 0); glVertex3f(upperLeft[0], lowerRight[1], 0);
    glTexCoord2f(1, 1); glVertex3f(lowerRight[0], upperLeft[1], 0);
    glTexCoord2f(1, 0); glVertex3f(lowerRight[0], lowerRight[1], 0);
    glEnd();

    m_textureMgr->SetUnit(0, false);
}

void OrthographicRenderer::RenderRectangle(const math::Rectangle& rect,
                                 const math::Vector2& u,
                                 const math::Vector2& v,
                                 const Color& diffuse,
                                 const TextureResourceHandleBase& texture)
{
    PushAttribute attr(GL_CURRENT_BIT);
    PushLoadModelViewMatrix modelview();

    m_textureMgr->BindTexture(0, texture);
    m_textureMgr->SetUnit(0, true);
    glColor4f(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);

    const Vector2& upperLeft = rect.Origin();
    const Vector2 lowerRight = upperLeft + rect.Extents();

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(u[0], 1.f - v[0]); glVertex3f(upperLeft[0], upperLeft[1], 0);
    glTexCoord2f(u[0], 1.f - v[1]); glVertex3f(upperLeft[0], lowerRight[1], 0);
    glTexCoord2f(u[1], 1.f - v[0]); glVertex3f(lowerRight[0], upperLeft[1], 0);
    glTexCoord2f(u[1], 1.f - v[1]); glVertex3f(lowerRight[0], lowerRight[1], 0);
    glEnd();

    m_textureMgr->SetUnit(0, false);
}

void OrthographicRenderer::RenderRectangle(const math::Rectangle& rect,
                                           const math::Rectangle& uv,
                                           const Color& diffuse,
                                           const TextureResourceHandleBase& texture)
{
    RenderRectangle(rect, math::Vector2(uv.Origin()[0], uv.Origin()[0] + uv.Width()),
                    math::Vector2(uv.Origin()[1], uv.Origin()[1] + uv.Height()),
                    diffuse,
                    texture);
}

void OrthographicRenderer::RenderRectangle(const math::Rectangle& rect,
                                           const real_t z,
                                           const math::Rectangle& uv,
                                           const Color& diffuse,
                                           const TextureResourceHandleBase& texture)
{
    PushAttribute attr(GL_CURRENT_BIT);
    PushLoadModelViewMatrix modelview();

    m_textureMgr->BindTexture(0, texture);
    m_textureMgr->SetUnit(0, true);
    glColor4f(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);

    const Vector2& upperLeft = rect.Origin();
    const Vector2 lowerRight = upperLeft + rect.Extents();
    const Vector2 u(uv.Origin()[0], uv.Origin()[0] + uv.Width());
    const Vector2 v(uv.Origin()[1], uv.Origin()[1] + uv.Height());

    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2f(u[0], 1.f - v[0]); glVertex3f(upperLeft[0], upperLeft[1], z);
    glTexCoord2f(u[0], 1.f - v[1]); glVertex3f(upperLeft[0], lowerRight[1], z);
    glTexCoord2f(u[1], 1.f - v[0]); glVertex3f(lowerRight[0], upperLeft[1], z);
    glTexCoord2f(u[1], 1.f - v[1]); glVertex3f(lowerRight[0], lowerRight[1], z);
    glEnd();

    m_textureMgr->SetUnit(0, false);
}


void OrthographicRenderer::RenderCurve(const Vector2& offset, const Curve& curve,
                              const Color& diffuse, float thickness, int steps)
{
    RenderCurve(offset, curve, diffuse, thickness, 0.f, 1.f, steps);
}

void OrthographicRenderer::RenderCurve(const Vector2& offset, const Curve& curve,
                              const Color& diffuse, float thickness,
                              float start, float end, int steps)
{
    //! \TODO Evaluate the performance of GL_LINE_STRIP primitives
    //! vis-a-vis manual segment generation.  I suspect line strip
    //! will be faster, or the difference negligible, though.

    PushAttribute attr(GL_CURRENT_BIT);
    PushLoadModelViewMatrix modelview;

    start = Clamp(start, 0.0f, 1.0f);
    end = Clamp(end, 0.0f, 1.0f);


    glColor4f(diffuse[0], diffuse[1], diffuse[2], diffuse[3]);
    glLineWidth(thickness);

    glTranslatef(offset[0], offset[1], 0);

    float period = 1.0f / (float)steps;
    glBegin(GL_LINE_STRIP);

    float t = start;
    for (; t <= end; t += period)
    {
        Vector3 v = curve.Sample(t);
        glVertex3f(v[0], v[1], v[2]);
    }

    Vector3 endSample = curve.Sample(end);
    glVertex3f(endSample[0], endSample[1], endSample[2]);

    glEnd();
}

}
}
}
