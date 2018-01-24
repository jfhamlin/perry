#include "Render/OpenGL/Framebuffer.h"
#include "Render/OpenGL/Utilities.h"
#include "Utility/Assertions.h"
#include <GL/glu.h>
#include <cstring>
#include <string>

namespace romulus
{
namespace render
{
namespace opengl
{

void Framebuffer::Unbind()
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

Framebuffer::Framebuffer(uint_t width, uint_t height):
    m_width(width), m_height(height),
    m_depthBuffer(0), m_depthIsRenderbuffer(false), m_ownsDepthBuffer(true)
{
    glGenFramebuffersEXT(1, &m_framebuffer);
    ASSERT_OPENGL_STATE();

    memset(&m_colorTextures[0], 0, sm_maxColorAttachments * sizeof(uint_t));
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffersEXT(1, &m_framebuffer);
    glDeleteTextures(sm_maxColorAttachments, &m_colorTextures[0]);
    if (m_ownsDepthBuffer)
    {
        if (m_depthIsRenderbuffer)
            glDeleteRenderbuffersEXT(1, &m_depthBuffer);
        else
            glDeleteTextures(1, &m_depthBuffer);
    }
}

int Framebuffer::MaxColorTextures() const
{
    int maxAttachments;
    glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS_EXT, &maxAttachments);
    ASSERT_OPENGL_STATE();
    return maxAttachments;
}

void Framebuffer::AttachColorTexture(int attachmentPoint,
                                     GLint internalTextureFormat,
                                     GLenum format, GLenum type)
{
    ASSERT(attachmentPoint < sm_maxColorAttachments);
    ASSERT(attachmentPoint < MaxColorTextures());
    ASSERT(!m_colorTextures[attachmentPoint]);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_framebuffer);

    Generate2DTextureBuffer(&m_colorTextures[attachmentPoint],m_width, m_height,
                            internalTextureFormat, format, type);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_COLOR_ATTACHMENT0_EXT + attachmentPoint,
                              GL_TEXTURE_2D, m_colorTextures[attachmentPoint],
                              0);
    ASSERT_OPENGL_STATE();
}

void Framebuffer::AttachDepthBuffer(bool useRenderbuffer)
{
    ASSERT(!m_depthBuffer);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_framebuffer);

    if (useRenderbuffer)
    {
        glGenRenderbuffersEXT(1, &m_depthBuffer);
        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_depthBuffer);
        glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24,
                                 m_width, m_height);
        glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
                                     GL_DEPTH_ATTACHMENT_EXT,
                                     GL_RENDERBUFFER_EXT, m_depthBuffer);
        m_depthIsRenderbuffer = true;
    }
    else
    {
        Generate2DTextureBuffer(&m_depthBuffer, m_width, m_height,
                                GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT,
                                GL_FLOAT);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                                  GL_DEPTH_ATTACHMENT_EXT,
                                  GL_TEXTURE_2D, m_depthBuffer,
                                  0);
    }

    m_ownsDepthBuffer = true;
    ASSERT_OPENGL_STATE();
}

void Framebuffer::AttachDepthBuffer(uint_t buffer)
{
    ASSERT(!m_depthBuffer);
    ASSERT(buffer);

    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_framebuffer);

    glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
                              GL_DEPTH_ATTACHMENT_EXT,
                              GL_TEXTURE_2D, buffer,
                              0);
    m_depthBuffer = buffer;

    m_ownsDepthBuffer = false;
    m_depthIsRenderbuffer = false;

    ASSERT_OPENGL_STATE();
}

void Framebuffer::Bind() const
{
    glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_framebuffer);

    GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
    if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
    {
        std::string errorMessage = "Cannot bind incomplete framebuffer: ";

        switch (status)
        {
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
                errorMessage += "Attachment.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
                errorMessage += "Missing attachment.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DUPLICATE_ATTACHMENT_EXT:
                errorMessage += "Duplicate attachment.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
                errorMessage += "Dimensions.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
                errorMessage += "Formats.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
                errorMessage += "Draw buffer.";
                break;
            case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
                errorMessage += "Read buffer.";
                break;
            case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
                errorMessage += "Unsupported.";
                break;
            default:
                errorMessage += "Status error.";
                break;
        }

        throw IncompleteFramebufferException(errorMessage);
    }

    ASSERT_OPENGL_STATE();
}

uint_t Framebuffer::ColorTextureGLHandle(int attachmentPoint) const
{
    return m_colorTextures[attachmentPoint];
}

}
}
}
