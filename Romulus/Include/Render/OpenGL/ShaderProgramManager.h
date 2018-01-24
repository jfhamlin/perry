#ifndef _OPENGLSHADERPROGRAMMANAGER_H_
#define _OPENGLSHADERPROGRAMMANAGER_H_

//! \file ShaderProgramManager.h
//! Contains the definition of the ShaderProgramManager class, which serves as
//! a central store for all of the ShaderPrograms used in the program.

#include "File/BasicFileManager.h"
#include "Utility/Common.h"
#include <boost/shared_ptr.hpp>
#include <map>
#include <string>

namespace romulus
{
namespace render
{
namespace opengl
{

class ShaderProgram;

//! Class for loading and storing GLSL shader programs.
class ShaderProgramManager
{
PROHIBIT_COPYING(ShaderProgramManager);
public:

    ShaderProgramManager();

    ShaderProgram* RequestShaderProgram(const std::string& vertexProgramPath,
                                        const std::string& fragmentProgramPath);

private:

    void LoadShaderProgramFile(std::string& contents, std::string path);

    typedef std::map<std::pair<std::string, std::string>,
                     boost::shared_ptr<ShaderProgram> > ShaderProgramMap;

    ShaderProgramMap m_shaderPrograms;

    BasicFileManager m_fileManager;
};

}
}
}

#endif // _OPENGLSHADERPROGRAMMANAGER_H_
