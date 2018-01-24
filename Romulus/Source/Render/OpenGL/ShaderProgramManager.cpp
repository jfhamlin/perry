#include "Render/OpenGL/ShaderProgram.h"
#include "Render/OpenGL/ShaderProgramManager.h"

namespace romulus
{
namespace render
{
namespace opengl
{

ShaderProgramManager::ShaderProgramManager():
    //! \todo We need a mechanism for setting this for release builds, as well
    //!       as for deploying the shader files to wherever they'll end up.
    m_fileManager("..:Romulus:Source:Render:OpenGL:Shaders")
{
}

ShaderProgram* ShaderProgramManager::RequestShaderProgram(
        const std::string& vertexProgramPath,
        const std::string& fragmentProgramPath)
{
    ShaderProgramMap::key_type key = std::make_pair(vertexProgramPath,
                                                    fragmentProgramPath);

    ShaderProgramMap::iterator it = m_shaderPrograms.find(key);


    if (it != m_shaderPrograms.end())
        return it->second.get();

    std::string vertexProgramContents;
    std::string fragmentProgramContents;

    LoadShaderProgramFile(vertexProgramContents, vertexProgramPath);
    LoadShaderProgramFile(fragmentProgramContents, fragmentProgramPath);

    boost::shared_ptr<ShaderProgram> shaderProgram;
    try
    {
        shaderProgram.reset(new ShaderProgram(vertexProgramContents,
                                              fragmentProgramContents));
    }
    catch (InvalidShaderProgram& e)
    {
        std::string exceptionMessage =
                "Could not compile shader programs <'" + vertexProgramPath +
                "', '" + fragmentProgramPath + "'>:\n";
        exceptionMessage += e.what();
        throw InvalidShaderProgram(exceptionMessage);
    }

    m_shaderPrograms.insert(make_pair(key, shaderProgram));

    return shaderProgram.get();
}

void ShaderProgramManager::LoadShaderProgramFile(std::string& contents,
                                                 std::string path)
{
    boost::shared_ptr<std::istream> shaderFileStream;

    if (!m_fileManager.ReadFile(path, shaderFileStream))
        throw InvalidShaderProgram("Unable to read file: " + path);

    ASSERT(shaderFileStream);

    std::string line;
    while (shaderFileStream->good())
    {
        std::getline(*shaderFileStream, line);
        contents += line + "\n";
    }
}

}
}
}
