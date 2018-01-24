#ifndef _RESOURCEFACTORY_H_
#define _RESOURCEFACTORY_H_

#include "Render/GeometryChunk.h"
#include "Render/IRenderDevice.h"
#include "Render/Light.h"
#include "Render/Material.h"
#include <boost/shared_ptr.hpp>
#include <istream>
#include <string>

namespace romulus
{

//! ResourceFactory class loads resources from files and archives.
//! Maintains a list of search paths where resource files are looked for.
class ResourceFactory
{
public:

    ResourceFactory(IRenderDevice& renderDevice,
                    const std::string& searchPath = "."):
        m_renderDevice(renderDevice)
    {
        m_searchPaths.push_back(searchPath);
    }

    //! Add a search path to the list of search paths. Paths added
    //! later are searched earlier.
    void AddSearchPath(const std::string& path)
    {
        m_searchPaths.push_back(path);
    }

    render::GeometryChunkPtr LoadOBJ(const std::string& fileName);

private:

    //! Search through m_searchPaths for file fileName and return a
    //! pointer to an istream for it.
    boost::shared_ptr<std::istream> OpenFile(const std::string& fileName);

    //! The render device to use when loading texture resources.
    IRenderDevice& m_renderDevice;

    //! The list of paths on which to search for files.
    std::vector<std::string> m_searchPaths;
};

//! Factory functions for geometry.
render::GeometryChunkPtr MakeGeometryChunk();
render::GeometryChunkPtr MakeSphere(const math::Vector3& center, real_t radius);

//! Factory functions for lights.
render::PointLightPtr MakePointLight();

//! Factory functions for materials.
render::MaterialPtr MakeMaterial();

} // namespace romulus

#endif // _RESOURCEFACTORY_H_
