#include "Resource/ResourceManager.h"
#include "Utility/Assertions.h"
#include <list>
#include <fstream>

namespace romulus
{
typedef boost::mutex::scoped_lock Lock;

ResourceManager::ResourceManager(IFileManager* fileMgr):
    m_fileMgr(fileMgr), m_nextUid(1)
{
    ASSERT(m_fileMgr);

    Log::Lock lock(m_log);
    m_log.RegisterSink(new std::ofstream("ResourceManager.log"), true,
        Log::MessageLevel_All);
    m_log << Log::MessageLevel_Info << "Resource manager constructed.\n";
}

ResourceManager::~ResourceManager()
{
    Log::Lock lock(m_log);
    m_log << Log::MessageLevel_Info << "Resource manager shutting down..."
        "releasing remaining resources...\n";
    ReleaseAll();
}

void ResourceManager::AcquireResource(const ResourceHandleBase& resource)
{
    Lock lock(m_lock);

    void* id = resource.m_id;
    ASSERT(id != 0);
    ASSERT(resource.m_manager == this);

    ResourceDescriptor* descriptor =
        reinterpret_cast<ResourceDescriptor*>(resource.m_id);

    ++descriptor->References;
}

void ResourceManager::ReleaseResource(const ResourceHandleBase& resource)
{
    void* id = resource.m_id;
    ASSERT(id != 0);
    ASSERT(resource.m_manager == this);

    ResourceDescriptor* descriptor =
            reinterpret_cast<ResourceDescriptor*>(resource.m_id);

    if ( --descriptor->References == 0)
    {
        if (ProviderRegistered(descriptor->Provider))
        {
            if (!descriptor->Procedural)
            {
                Lock lock(m_lock);
                Log::Lock logLock(m_log);
                m_log << Log::MessageLevel_Info << "<" << descriptor->Path
                      << "> no longer referenced, releasing.\n";

                m_streamResources.erase(descriptor->Path);
            }
            else
            {
                Lock lock(m_lock);
                Log::Lock logLock(m_log);
                m_log << Log::MessageLevel_Info << "Procedural resource on"
                      << " provider <" << descriptor->Provider << "> no"
                      << " longer referenced, releasing.\n";

                m_proceduralResources.erase(
                        std::find(m_proceduralResources.begin(),
                                  m_proceduralResources.end(), descriptor));
            }

            descriptor->Provider->UnloadResource(descriptor->Id);
        }
        delete descriptor;
    }
}

ResourceHandleBase::id_t ResourceManager::ProviderID(
    const ResourceHandleBase& resource) const
{
    ASSERT(resource.m_manager == this);
    return reinterpret_cast<ResourceDescriptor*>(resource.m_id)->Id;
}

ResourceHandleBase::uid_t ResourceManager::UniqueID(const ResourceHandleBase& resource) const
{
    ASSERT(resource.m_manager == this);
    return reinterpret_cast<ResourceDescriptor*>(resource.m_id)->Uid;
}

IResourceProvider* ResourceManager::GetProvider(const ResourceHandleBase& resource) const
{
    ASSERT(resource.m_manager == this);
    return reinterpret_cast<ResourceDescriptor*>(resource.m_id)->Provider;
}

void ResourceManager::ReleaseAll()
{
    Lock lock(m_lock);
    {
        Log::Lock logLock(m_log);
        for (DescriptorMap::iterator iter = m_streamResources.begin();
             iter != m_streamResources.end(); ++iter)
        {
            ResourceDescriptor* descriptor = iter->second;
            m_log << Log::MessageLevel_Info << "<" << descriptor->Path
                  << "> releasing.\n";
            descriptor->Provider->UnloadResource(descriptor->Id);
            delete descriptor;
        }
    }
    m_streamResources.clear();

    for (DescriptorCollection::iterator iter = m_proceduralResources.begin();
        iter != m_proceduralResources.end(); ++iter)
    {
        ResourceDescriptor* descriptor = *iter;
        descriptor->Provider->UnloadResource(descriptor->Id);
        delete descriptor;
    }
    m_proceduralResources.clear();
}

void ResourceManager::RegisterProvider(IStreamResourceProvider* provider)
{
    ASSERT(provider);
    Lock lock(m_lock);

    int resourceType = provider->HandleType();

    {
        Log::Lock logLock(m_log);
        m_log << Log::MessageLevel_Info << "Registering new provider <" <<
                provider << ">, handling resource type <";
        m_log << resourceType << ">.\n";
    }

    StreamProvidersMap::iterator iter = m_streamProviders.find(resourceType);
    if (iter == m_streamProviders.end())
    {
        m_streamProviders[resourceType] = StreamProviderMap();
        iter = m_streamProviders.find(resourceType);
    }

    const IStreamResourceProvider::ExtensionCollection& extensions =
            provider->HandledExtensions();
    for (IStreamResourceProvider::ExtensionCollection::const_iterator extension
                 = extensions.begin(); extension != extensions.end();
         ++extension)
    {
        iter->second[*extension] = provider;
    }

    m_providers.push_back(provider);
}

void ResourceManager::UnregisterProvider(IStreamResourceProvider* provider)
{
    ASSERT(provider);
    Lock lock(m_lock);

    {
        Log::Lock logLock(m_log);
        m_log << Log::MessageLevel_Info << "Unregistering provider <" <<
                provider << ">, releasing associated resources: <";

        bool first = true;
        for (DescriptorMap::iterator iter = m_streamResources.begin();
             iter != m_streamResources.end(); ++iter)
        {
            ResourceDescriptor* descriptor = iter->second;
            if (descriptor->Provider != provider)
                continue;

            if (first)
            {
                m_log << iter->first;
                first = false;
            }
            else
                m_log << ", " << iter->first;

            DescriptorMap::iterator temp = iter;
            iter = iter--;
            m_streamResources.erase(temp);
            descriptor->Provider->UnloadResource(descriptor->Id);
            delete descriptor;
        }

        m_log << ">.\n";
    }

    int resourceType = provider->HandleType();
    StreamProvidersMap::iterator map = m_streamProviders.find(resourceType);
    ASSERT(map != m_streamProviders.end());

    const IStreamResourceProvider::ExtensionCollection& extensions =
            provider->HandledExtensions();
    for (IStreamResourceProvider::ExtensionCollection::const_iterator extension
                 = extensions.begin(); extension != extensions.end();
         ++extension)
    {
        StreamProviderMap::iterator iter = map->second.find(*extension);
        if (iter != map->second.end() && iter->second == provider)
            map->second.erase(iter);
    }

    std::remove(m_providers.begin(), m_providers.end(), provider);
}

void ResourceManager::RegisterProvider(IProceduralResourceProvider* provider)
{
    ASSERT(provider);
    Lock lock(m_lock);

    int resourceType = provider->HandleType();
    ProceduralProvidersMap::iterator iter =
            m_proceduralProviders.find(resourceType);
    if (iter == m_proceduralProviders.end())
    {
        m_proceduralProviders[resourceType] = ProceduralProviderMap();
        iter = m_proceduralProviders.find(resourceType);
    }

    iter->second[&provider->HandledArgumentType()] = provider;
    m_providers.push_back(provider);
}

void ResourceManager::UnregisterProvider(IProceduralResourceProvider* provider)
{
    ASSERT(provider);
    Lock lock(m_lock);

    for (DescriptorCollection::iterator iter = m_proceduralResources.begin();
        iter != m_proceduralResources.end(); ++iter)
    {
        ResourceDescriptor* descriptor = *iter;
        if (descriptor->Provider == provider)
        {
            provider->UnloadResource(descriptor->Id);
            delete descriptor;

            DescriptorCollection::iterator temp = iter;
            iter--;
            m_proceduralResources.erase(temp);
        }
    }

    int resourceType = provider->HandleType();
    ProceduralProvidersMap::iterator iter =
            m_proceduralProviders.find(resourceType);
    ASSERT(iter != m_proceduralProviders.end());

    iter->second.erase(iter->second.find(&provider->HandledArgumentType()));

    std::remove(m_providers.begin(), m_providers.end(), provider);
}

bool ResourceManager::ProviderRegistered(const IResourceProvider* provider) const
{
    return std::find(m_providers.begin(), m_providers.end(), provider)
            != m_providers.end();
}

ResourceManager::ResourceDescriptor* ResourceManager::LoadStreamResource(
    int resourceType, const std::string& path)
{
    boost::shared_ptr<std::istream> stream;
    IStreamResourceProvider* provider;
    // Check if the resource is resident.
    {
        Lock lock(m_lock);
        DescriptorMap::iterator elementIter = m_streamResources.find(path);
        if (elementIter != m_streamResources.end())
        {
            ++elementIter->second->References;
            return elementIter->second;
        }

        StreamProvidersMap::iterator iter = m_streamProviders.find(
                resourceType);
        if (iter == m_streamProviders.end())
            throw UnsupportedResourceType();

        std::string extension = path.substr(path.find_last_of('.') + 1,
                                            path.size());

        StreamProviderMap::iterator providerIter = iter->second.find(extension);
        if (providerIter == iter->second.end())
            throw UnsupportedResourceType();
        provider = providerIter->second;

        if (!m_fileMgr->ReadFile(path, stream))
            throw InvalidPathException();
    }

    ResourceHandleBase::id_t id;
    if (!provider->LoadResource(*stream, id))
    {
        //! \todo Report meaningful information about the exception, please.
        throw InvalidResource();
    }

    ResourceDescriptor* descriptor = new ResourceDescriptor;
    descriptor->Uid = m_nextUid++;
    descriptor->Id = id;
    descriptor->Provider = provider;
    descriptor->References = 1;
    descriptor->Path = path;
    descriptor->Procedural = false;

    {
        Lock lock(m_lock);
        m_streamResources[path] = descriptor;
    }

    return descriptor;
}

ResourceManager::ResourceDescriptor* ResourceManager::LoadProceduralResource(
    int resourceType, const ProceduralResourceArguments& arguments)
{
    Lock lock(m_lock);
    ProceduralProvidersMap::iterator iter =
            m_proceduralProviders.find(resourceType);
    if (iter == m_proceduralProviders.end())
        throw UnsupportedResourceType();

    const RTTI* type = &arguments.Type();
    ProceduralProviderMap::iterator providerIter = iter->second.find(type);
    if (providerIter == iter->second.end())
        throw UnsupportedResourceType();

    IProceduralResourceProvider* provider = providerIter->second;

    ResourceHandleBase::id_t id;
    if (!provider->GenerateResource(arguments, id))
        throw InvalidResource();

    ResourceDescriptor* descriptor = new ResourceDescriptor;
    descriptor->Uid = m_nextUid++;
    descriptor->Id = id;
    descriptor->Provider = provider;
    descriptor->References = 1;
    descriptor->Procedural = true;

    m_proceduralResources.push_back(descriptor);

    return descriptor;
}

}
