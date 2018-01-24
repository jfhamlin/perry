#ifndef _RENDEROPENGLFRAMEBUFFER_H_
#define _RENDEROPENGLFRAMEBUFFER_H_

//! \file Framebuffer.h
//! Contains the declaration of the Framebuffer OpenGL utility class.

#include "Core/Types.h"
#include "Render/OpenGL/GLee.h"
#include <exception>
#include <string>

namespace romulus
{
namespace render
{
namespace opengl
{

class IncompleteFramebufferException : public std::exception
{
public:
    inline IncompleteFramebufferException() { }
    inline IncompleteFramebufferException(const std::string& message):
        m_message(message) { }
    virtual ~IncompleteFramebufferException() throw() { }
    virtual const char* what() const throw() { return m_message.c_str(); }

private:
    std::string m_message;
};

class Framebuffer
{
public:

    //! Unbind any currently bound Framebuffer.
    static void Unbind();

    //! \param i - The index of the attachment point GLenum desired.
    //! \return The GL enumeration value for the ith attachment point.
    static GLenum ColorTextureAttachmentEnum(int i)
    { return GL_COLOR_ATTACHMENT0_EXT + i; }

    Framebuffer(uint_t width, uint_t height);
    ~Framebuffer();

    //! \return The width of this Framebuffer.
    uint_t Width() const { return m_width; }

    //! \return The height of this Framebuffer.
    uint_t Height() const { return m_height; }

    //! \return The maximum number of color textures one may attach.
    int MaxColorTextures() const;

    //! Create and attach a color buffer to this Framebuffer.
    //! \param attachmentPoint - the attachment point, in
    //!                          [0, MaxColorTextures()), at which to attach
    //!                          the new texture.
    //! \param internalTextureFormat - the number of color components in the
    //!                                texture, e.g. GL_RGBA16.
    //! \param format - the format of the texture's pixel data, e.g. GL_RGBA.
    //! \param type - the data type of the pixel data, e.g. GL_FLOAT.
    void AttachColorTexture(int attachmentPoint, GLint internalTextureFormat,
                            GLenum format, GLenum type);

    //! \param attachmentPoint - the attachment point, in
    //!                          [0, MaxColorTextures()), whose texture handle
    //!                          should be returned.
    //! \return The GL handle of the color texture attached at attachmentPoint.
    uint_t ColorTextureGLHandle(int attachmentPoint) const;

    //! Create and attach a depth buffer to this Framebuffer.
    //! \param useRenderbuffer - if useRenderbuffer is true, the depth buffer
    //!                          will be created using a renderbuffer. Else,
    //!                          it will be created using a texture.
    void AttachDepthBuffer(bool useRenderbuffer);

    //! Attach an existing GL texture as a depth buffer.  Ownership of the
    //! attached buffer belongs to the caller--Framebuffer will not delete
    //! the texture upon destruction.
    //! \param buffer - A handle to an existing GL depth buffer as this
    //!                 framebuffer's depth buffer.
    void AttachDepthBuffer(uint_t buffer);

    bool DepthBufferIsRenderbuffer() { return m_depthIsRenderbuffer; }

    //! \return The GL handle to the depth buffer renderbuffer.
    uint_t DepthBufferGLHandle() const { return m_depthBuffer; }

    //! Bind the Framebuffer.
    //! \throw IncompleteFramebufferException if Framebuffer cannot be bound.
    void Bind() const;

private:
    //! The maximum number of color attachment points allowed by the
    //! specification, which is >= the number actually supported.
    static const int sm_maxColorAttachments = 16;

    //! Buffer width of all attached textures.
    uint_t m_width;
    //! Buffer height of all attached textures.
    uint_t m_height;
    //! OpenGL handle for this framebuffer.
    uint_t m_framebuffer;
    //! OpenGL handle of the depth buffer, or zero.
    uint_t m_depthBuffer;
    //! True if m_depthBuffer is a renderbuffer handle.
    bool m_depthIsRenderbuffer;
    //! True if this Framebuffer owns the depthbuffer texture.
    bool m_ownsDepthBuffer;
    //! Array of OpenGL handles of the color textures.
    uint_t m_colorTextures[sm_maxColorAttachments];
};

}
}
}

#endif // _RENDEROPENGLFRAMEBUFFER_H_
