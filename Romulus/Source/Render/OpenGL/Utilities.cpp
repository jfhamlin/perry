#include "Render/OpenGL/Utilities.h"
#include "Platform/Platform.h"
#include <GL/glu.h>
#include <boost/lexical_cast.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

void Generate2DTextureBuffer(uint_t* bufferHandle, GLsizei width,
                             GLsizei height, GLint internalTextureFormat,
                             GLenum format, GLenum type)
{
    glGenTextures(1, bufferHandle);
    glBindTexture(GL_TEXTURE_2D, *bufferHandle);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, internalTextureFormat, width, height, 0,
                 format, type, 0);
}

void RenderQuad(real_t minX, real_t minY, real_t width, real_t height)
{
    glBegin(GL_QUADS);
    glTexCoord2f(0, 1); glVertex3f(minX, minY, 0);
    glTexCoord2f(0, 0); glVertex3f(minX, minY + height, 0);
    glTexCoord2f(1, 0); glVertex3f(minX + width, minY + height, 0);
    glTexCoord2f(1, 1); glVertex3f(minX + width, minY, 0);
    glEnd();
}

void AssertOpenGLState(const char* file, size_t line)
{
    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
        std::string glMessage =
                reinterpret_cast<const char*>(gluErrorString(error));

        std::string message = file;
        message += ":";
        message += boost::lexical_cast<std::string>(line);
        message += "\n" + glMessage;
        message += "Backtrace:\n";
        message += platform::Backtrace();

        throw InvalidOpenGLStateException(message);
    }
}

} // namespace opengl
} // namespace render
} // namespace romulus
