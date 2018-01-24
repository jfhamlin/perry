#ifndef _IRESOURCEPROVIDER_H_
#define _IRESOURCEPROVIDER_H_

//! \file IResourceProvider.h
//! Contains resource provider interface.

#include "Core/RTTI.h"
#include "Resource/ResourceHandle.h"
#include <vector>
#include <string>
#include <iostream>

namespace romulus
{
//! Resource provider interface.
class IResourceProvider
{
public:

    IResourceProvider() { }
    virtual ~IResourceProvider() { }

    //! Unload the specified resource.
    //! \param id - Identifier of the resource to unload.
    virtual void UnloadResource(ResourceHandleBase::id_t id) = 0;

    virtual int HandleType() const = 0;

    virtual void* GetResource(ResourceHandleBase::id_t id) = 0;
};

class IStreamResourceProvider : public IResourceProvider
{
public:

    typedef std::vector<std::string> ExtensionCollection;

    //! Attempts to load a resource from the given file stream.
    //! \param stream - The file stream from whence to load.
    //! \param id - Provider provided resource id--must not be
    //! ResourceHandleBase::NullId.
    virtual bool LoadResource(std::istream& stream,
        ResourceHandleBase::id_t& id) = 0;

    virtual const ExtensionCollection& HandledExtensions() const = 0;
};

class ProceduralResourceArguments
{
DECLARE_BASE_RTTI;
public:
    virtual ~ProceduralResourceArguments() { }
};

class IProceduralResourceProvider : public IResourceProvider
{
public:

    //! Attempts to generate the resource given the arguments.
    //! \param arguments - The arguments of the particular resource.
    //! \param id - Provider provided resource id--must not be
    //! ResourceHandleBase::NullId.
    virtual bool GenerateResource(
        const ProceduralResourceArguments& arguments,
        ResourceHandleBase::id_t& id) = 0;

    virtual const RTTI& HandledArgumentType() const = 0;
};

}

#endif // _IRESOURCEPROVIDER_H_
