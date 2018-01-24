#include "Resource/ResourceHandle.h"
#include "Resource/ResourceManager.h"

namespace romulus
{
int ResourceHandleBase::sm_nextHandleID = 1;
const ResourceHandleBase::id_t ResourceHandleBase::NullId = 0;

ResourceHandleBase::ResourceHandleBase():
    m_id(0), m_manager(0)
{
}

ResourceHandleBase::id_t ResourceHandleBase::ID() const
{
    if (!m_id)
        throw NullResourceException();

    return m_manager->ProviderID(*this);
}

ResourceHandleBase::uid_t ResourceHandleBase::UID() const
{
    if (!m_id)
        throw NullResourceException();

    return m_manager->UniqueID(*this);
}

void ResourceHandleBase::Assign(const ResourceHandleBase& source)
{
    if (m_id)
        Release();

    m_id = source.m_id;
    m_manager = source.m_manager;

    if (m_id)
        m_manager->AcquireResource(source);
}

void ResourceHandleBase::Release()
{
    m_manager->ReleaseResource(*this);
    m_id = 0;
    m_manager = 0;
}

void* ResourceHandleBase::LookupResource() const
{
    if (!m_id)
        throw NullResourceException();
    return m_manager->GetProvider(*this)->GetResource(ID());
}
}
