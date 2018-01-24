#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/GLee.h"
#include <boost/scoped_array.hpp>

namespace romulus
{
namespace render
{
namespace opengl
{

void ShaderProgram::Unbind()
{
    glUseProgram(0);
}

ShaderProgram::ShaderProgram(const std::string& vertexProgram,
                             const std::string& fragmentProgram)
{
    m_program = glCreateProgram();

    LoadProgram(vertexProgram, GL_VERTEX_SHADER, m_vertexProgram);
    LoadProgram(fragmentProgram, GL_FRAGMENT_SHADER, m_fragmentProgram);

    glAttachShader(m_program, m_vertexProgram);
    glAttachShader(m_program, m_fragmentProgram);

    glLinkProgram(m_program);

    int success;
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);

    if (!success)
    {
        int logLength, written;
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logLength);

        boost::scoped_array<char> log(new char[logLength + 1]);
        glGetProgramInfoLog(m_program, logLength, &written, &log[0]);

        throw InvalidShaderProgram(&log[0]);
    }
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_program);
    glDeleteShader(m_vertexProgram);
    glDeleteShader(m_fragmentProgram);
}

void ShaderProgram::Bind() const
{
    glUseProgram(m_program);
}


void ShaderProgram::SetUniformParameter(const std::string& name,
                                        real_t param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform1f(location, param);
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        int param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform1i(location, param);
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const math::Vector2& param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform2f(location, param[0], param[1]);
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const math::Vector3& param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform3f(location, param[0], param[1], param[2]);
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const Color& param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    glUniform4f(location, param[0], param[1], param[2], param[3]);
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const math::Matrix33& param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    math::Matrix<3, 3, float> m(param);
    glUniformMatrix3fv(location, 1, GL_TRUE, m.Data());
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const math::Matrix44& param) const
{
    int location = glGetUniformLocation(m_program, name.c_str());
    math::Matrix<4, 4, float> m(param);
    glUniformMatrix4fv(location, 1, GL_TRUE, m.Data());
}

void ShaderProgram::SetUniformParameter(const std::string& name,
                                        const uint_t textureUnit) const
{
    int location = glGetUniformLocationARB(m_program, name.c_str());
    glUniform1iARB(location, textureUnit);
}

int ShaderProgram::LookupAttributeLocation(const std::string& attributeName) const
{
    return glGetAttribLocation(m_program, attributeName.c_str());
}

void ShaderProgram::LoadProgram(const std::string& program, int type, uint_t& object)
{
    object = glCreateShader(type);

    const char* source[1];
    source[0] = program.c_str();
    glShaderSource(object, 1, source, 0);

    glCompileShader(object);

    int success;
    glGetShaderiv(object, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        int logLength, written;
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &logLength);

        boost::scoped_array<char> log(new char[logLength + 1]);
        glGetShaderInfoLog(object, logLength, &written, &log[0]);

        throw InvalidShaderProgram(&log[0]);
    }
}

}
}
}
