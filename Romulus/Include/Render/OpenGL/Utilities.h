#ifndef _RENDEROPENGLUTILITIES_H_
#define _RENDEROPENGLUTILITIES_H_

//! \file Utilities.h
//! Contains RAII classes for convenient pushing/popping OpenGL matrices,
//! enabling/disabling various states, and creating OpenGL color and depth
//! buffers.

#include "Core/Types.h"
#include "Math/Matrix.h"
#include "Render/OpenGL/GLee.h"

namespace romulus
{
namespace render
{
namespace opengl
{

const GLenum GL_ROMULUS_REAL =
        sizeof(real_t) == sizeof(float) ? GL_FLOAT : GL_DOUBLE;

inline void LoadTransposeMatrix(const float* data)
{
    glLoadTransposeMatrixfARB(data);
}

inline void LoadTransposeMatrix(const double* data)
{
    glLoadTransposeMatrixdARB(data);
}

inline void MultTransposeMatrix(const float* data)
{
    glMultTransposeMatrixfARB(data);
}

inline void MultTransposeMatrix(const double* data)
{
    glMultTransposeMatrixdARB(data);
}

class PushLoadProjectionMatrix
{
public:

    PushLoadProjectionMatrix(const math::Matrix44& projectionMatrix)
    {
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        LoadTransposeMatrix(projectionMatrix.Data());
    }

    ~PushLoadProjectionMatrix()
    {
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
    }
};

class PushLoadModelViewMatrix
{
public:

    PushLoadModelViewMatrix()
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        glLoadIdentity();
    }

    PushLoadModelViewMatrix(const math::Matrix44& modelViewMatrix)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        LoadTransposeMatrix(modelViewMatrix.Data());
    }

    ~PushLoadModelViewMatrix()
    {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
};

class PushMultiplyModelViewMatrix
{
public:

    PushMultiplyModelViewMatrix(const math::Matrix44& modelViewMatrix)
    {
        glMatrixMode(GL_MODELVIEW);
        glPushMatrix();
        MultTransposeMatrix(modelViewMatrix.Data());
    }

    ~PushMultiplyModelViewMatrix()
    {
        glMatrixMode(GL_MODELVIEW);
        glPopMatrix();
    }
};

class PushAttribute
{
public:
    PushAttribute(uint_t glAttributeBits)
    {
        glPushAttrib(glAttributeBits);
    }

    ~PushAttribute()
    {
        glPopAttrib();
    }
};

//! Generates an OpenGL 2D texture with a given size and format. Standard
//! settings are chosen for filtering (GL_NEAREST) and wrapping (GL_CLAMP).
//! In creating the texture, the texture is bound with glBindTexture(), so
//! texture parameters for the texture may be set and overridden as desired
//! after this function returns.
//! \param bufferHandle - a pointer in which the GLHandle to the buffer is
//!                       stored.
//! \param width - the width of the generated texture.
//! \param height - the height of the generated texture.
//! \param internalTextureFormat - the format of the texture, specifying the
//!                                number of color components or depth.
//!                                (e.g. GL_ALPHA, GL_RGBA16F_ARB,
//!                                      GL_DEPTH_COMPONENT24)
//! \param format - the format of the pixel data.
//!                 (e.g. GL_RGBA, GL_DEPTH_COMPONENT)
//! \param type - the data type of the pixel data. (e.g. GL_FLOAT, GL_BYTE)
//!
void Generate2DTextureBuffer(uint_t* bufferHandle, GLsizei width,
                             GLsizei height, GLint internalTextureFormat,
                             GLenum format, GLenum type);

//! Render a quad on the x-y plane with texture coordinates (0, 0) at
//! (minX, minY + height) and (1, 1) at (minX + width, minY).
//! \param minX - the minimum x boundary of the quad.
//! \param minY - the minimum y boundary of the quad.
//! \param width - the width of the quad.
//! \param height - the height of the quad.
void RenderQuad(real_t minX, real_t minY, real_t width, real_t height);

class InvalidOpenGLStateException : public std::exception
{
public:

    InvalidOpenGLStateException(const std::string& msg):
        m_message(msg)
    {
    }

    virtual ~InvalidOpenGLStateException() throw() { }

    virtual const char* what() const throw() { return m_message.c_str(); }

private:

    std::string m_message;
};

void AssertOpenGLState(const char* file, size_t line);

#ifdef ENABLE_ASSERTIONS
#define ASSERT_OPENGL_STATE() AssertOpenGLState(__FILE__, __LINE__)
#else
#define ASSERT_OPENGL_STATE()
#endif

} // namespace opengl
} // namespace render
} // namespace romulus

#endif // _RENDEROPENGLUTILITIES_H_
