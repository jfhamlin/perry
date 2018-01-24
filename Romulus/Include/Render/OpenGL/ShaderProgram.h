#ifndef _RENDEROPENGLSHADERPROGRAM_H_
#define _RENDEROPENGLSHADERPROGRAM_H_

//! \file ShaderProgram.h
//! Contains the declaration of the shader program OpenGL utility class.

#include "Core/Types.h"
#include "Math/Vector.h"
#include "Math/Matrix.h"
#include "Render/OpenGL/TextureManager.h"
#include "Render/Color.h"
#include <string>

namespace romulus
{
namespace render
{
namespace opengl
{

class InvalidShaderProgram : public std::exception
{
public:

    inline InvalidShaderProgram() { }
    inline InvalidShaderProgram(const std::string& message):
        m_message(message) { }
    virtual ~InvalidShaderProgram() throw() { }

    virtual const char* what() const throw() { return m_message.c_str(); }

private:

    std::string m_message;
};

//! Shader program utility.
class ShaderProgram
{
public:

    //! Unbind any bound shader program.
    static void Unbind();

    //! Construct the program.
    //! \param vertexProgram - The vertex program source.
    //! \param fragmentProgram - The fragment program source.
    //! \throw InvalidShaderProgram upon error.
    ShaderProgram(const std::string& vertexProgram,
                  const std::string& fragmentProgram);
    ~ShaderProgram();

    //! Bind the shader program.
    void Bind() const;


    void SetUniformParameter(const std::string& name, real_t param) const;
    void SetUniformParameter(const std::string& name, int param) const;
    void SetUniformParameter(const std::string& name,
                             const math::Vector2& param) const;
    void SetUniformParameter(const std::string& name,
                             const math::Vector3& param) const;
    void SetUniformParameter(const std::string& name,
                             const Color& param) const;
    void SetUniformParameter(const std::string& name,
                             const math::Matrix33& param) const;
    void SetUniformParameter(const std::string& name,
                             const math::Matrix44& param) const;
    void SetUniformParameter(const std::string& name,
                             const uint_t textureUnit) const;

    int LookupAttributeLocation(const std::string& attributeName) const;

private:

    void LoadProgram(const std::string& program, int type, uint_t& object);

    uint_t m_program;
    uint_t m_vertexProgram;
    uint_t m_fragmentProgram;
};

}
}
}

#endif // _RENDEROPENGLSHADERPROGRAM_H_

