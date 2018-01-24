#ifndef _RESOURCESPHEREGEOMETRYPROVIDER_H_
#define _RESOURCESPHEREGEOMETRYPROVIDER_H_

#include "Core/Types.h"
#include "Resource/IResourceProvider.h"
#include <boost/shared_ptr.hpp>
#include <map>

namespace romulus
{
namespace render
{
class SimpleGeometryChunk; // Forward declaration of SimpleGeometryChunk.
} // namespace render

class ProceduralSphereArguments : public ProceduralResourceArguments
{
DECLARE_RTTI(ProceduralResourceArguments);
public:
    ProceduralSphereArguments():
        LongitudinalSegments(10), LatitudinalSegments(10) { }
    ProceduralSphereArguments(uint_t longitudinalSegments,
                              uint_t latitudinalSegments):
        LongitudinalSegments(longitudinalSegments),
        LatitudinalSegments(latitudinalSegments) { }

    uint_t LongitudinalSegments;
    uint_t LatitudinalSegments;
};

class SphereGeometryProvider : public IProceduralResourceProvider
{
public:

    SphereGeometryProvider(): m_nextResourceID(1) { }
    virtual ~SphereGeometryProvider() { }

    //! Unload the specified resource.
    //! \param id - Identifier of the resource to unload.
    virtual void UnloadResource(ResourceHandleBase::id_t id);

    virtual int HandleType() const;

    virtual void* GetResource(ResourceHandleBase::id_t id);

    virtual bool GenerateResource(
            const ProceduralResourceArguments& arguments,
            ResourceHandleBase::id_t& id);

    virtual const RTTI& HandledArgumentType() const
    {
        return ProceduralSphereArguments::TypeInformation;
    }

private:

    ResourceHandleBase::id_t CreateNewSphere(
            const std::pair<uint_t, uint_t>& lonLatSegments);

    typedef std::map<ResourceHandleBase::id_t,
                     std::pair<uint_t, uint_t> > ArgumentsMap;
    typedef std::map<std::pair<uint_t, uint_t>,
                     ResourceHandleBase::id_t> IDMap;
    typedef std::map<std::pair<uint_t, uint_t>,
                     boost::shared_ptr<render::SimpleGeometryChunk> > SphereMap;

    ArgumentsMap m_arguments;
    IDMap m_ids;
    SphereMap m_spheres;
    ResourceHandleBase::id_t m_nextResourceID;
};

} // namespace romulus

#endif // _RESOURCESPHEREGEOMETRYPROVIDER_H_
