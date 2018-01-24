#include "Resource/MutableGeometryProvider.h"

namespace romulus
{

IMPLEMENT_RTTI(MutableGeometryResourceArguments);

bool MutableGeometryProvider::GenerateResource(
            const ProceduralResourceArguments& arguments,
            ResourceHandleBase::id_t& id)
{
    const MutableGeometryResourceArguments& mutableGeometryArgs =
            static_cast<const MutableGeometryResourceArguments&>(arguments);
    const render::GeometryChunk* gc = mutableGeometryArgs.GC;

    MutableGeometryPtr mesh(new MutableGeometryChunk());

    if (gc)
    {
        // Copy the argument mesh into the mutable mesh.
        mesh->SetVertices(gc->Vertices(), gc->Vertices() + gc->VertexCount());
        for (uint_t i = 0; i < gc->IndexCount(); i += 3)
            mesh->AddFace(gc->Indices()[i],
                          gc->Indices()[i + 1],
                          gc->Indices()[i + 2]);
    }

    uint_t index = FindFreeSlot();
    m_meshes[index] = mesh;
    id = index + 1;

    return true;
}

void MutableGeometryProvider::UnloadResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);
    ASSERT(index < m_meshes.size());

    MutableGeometryChunk* mesh = m_meshes[index].get();
    ASSERT(mesh);

    m_meshes[index].reset();
}

void* MutableGeometryProvider::GetResource(ResourceHandleBase::id_t id)
{
    uint_t index = id - 1;
    ASSERT(index >= 0);
    ASSERT(index < m_meshes.size());

    MutableGeometryChunk* mesh = m_meshes[index].get();
    ASSERT(mesh);

    return reinterpret_cast<void*>(mesh);
}

uint_t MutableGeometryProvider::FindFreeSlot()
{
    for (uint_t i = 0; i < m_meshes.size(); ++i)
        if (!m_meshes[i].get())
            return i;

    m_meshes.push_back(MutableGeometryPtr());
    return m_meshes.size() - 1;
}

}
