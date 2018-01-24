#include "Math/Bounds/IBoundingVolume.h"

namespace romulus
{

/** Octree::Node member function definitions. */

template <class NodePayload>
const math::AABB& Octree<NodePayload>::Node::BoundingBox() const
{
    return Bounds;
}

template <class NodePayload>
typename Octree<NodePayload>::Node* Octree<NodePayload>::Node::Parent()
        const
{
    return m_parent;
}

template <class NodePayload>
typename Octree<NodePayload>::Node* Octree<NodePayload>::Node::Child(
        uint_t childIndex) const
{
    ASSERT(childIndex < 8u);
    return (&Children[0][0][0] + childIndex)->get();
}

template <class NodePayload>
typename Octree<NodePayload>::Node* Octree<NodePayload>::Node::Child(
        uint_t x, uint_t y, uint_t z) const
{
    ASSERT(x < 2);
    ASSERT(y < 2);
    ASSERT(z < 2);
    return Children[x][y][z].get();
}

template <class NodePayload>
Octree<NodePayload>::Node::Node(): m_parent(0)
{
}

/** Octree::Iterator member function definitions. */

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
typename Octree<NodePayload>::template Iterator<TraversalContainer>&
Octree<NodePayload>::Iterator<TraversalContainer>::operator++()
{
    ASSERT(m_octree);
    Node* currentNode = CurrentNode(m_nodes);
    m_nodes.pop();
    if (m_octree->HasChildren(currentNode))
        for (int i = 0; i < 8; ++i)
            m_nodes.push(currentNode->Child(i));
    return *this;
}

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
typename Octree<NodePayload>::Node&
Octree<NodePayload>::Iterator<TraversalContainer>::operator*() const
{
    ASSERT(CurrentNode(m_nodes));
    return *CurrentNode(m_nodes);
}

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
bool Octree<NodePayload>::Iterator<TraversalContainer>::operator==(
        const Iterator<TraversalContainer>& it) const
{
    if (m_nodes.size() && it.m_nodes.size())
        return CurrentNode(m_nodes) == it.CurrentNode(it.m_nodes);
    else if (!m_octree || !it.m_octree || m_octree != it.m_octree)
        return false;
    return m_nodes.size() == it.m_nodes.size();
}

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
bool Octree<NodePayload>::Iterator<TraversalContainer>::operator!=(
        const Iterator<TraversalContainer>& it) const
{
    return !(*this == it);
}

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
typename Octree<NodePayload>::Node*
Octree<NodePayload>::Iterator<TraversalContainer>::CurrentNode(
        const std::stack<typename Octree<NodePayload>::Node*>& nodes) const
{
    ASSERT(m_octree);
    ASSERT(nodes.size());
    return nodes.top();
}

template <class NodePayload>
template <template <typename, typename> class TraversalContainer>
typename Octree<NodePayload>::Node*
Octree<NodePayload>::Iterator<TraversalContainer>::CurrentNode(
        const std::queue<typename Octree<NodePayload>::Node*>& nodes) const
{
    ASSERT(m_octree);
    ASSERT(nodes.size());
    return nodes.front();
}

/** Octree member function definitions. */

template <class NodePayload>
Octree<NodePayload>::Octree():
    m_root(0)
{
    m_root = new Node;
    m_root->Bounds = math::AABB(math::Vector3(-100), math::Vector3(100));
}

template <class NodePayload>
Octree<NodePayload>::Octree(const math::AABB& rootBounds):
    m_root(0)
{
    m_root.reset(new Node);
    m_root->Bounds = rootBounds;
}

template <class NodePayload>
typename Octree<NodePayload>::DepthFirstIterator
Octree<NodePayload>::DepthFirstBegin() const
{
    DepthFirstIterator it;
    it.m_octree = this;
    it.m_nodes.push(m_root.get());
    return it;
}

template <class NodePayload>
typename Octree<NodePayload>::DepthFirstIterator
Octree<NodePayload>::DepthFirstEnd() const
{
    DepthFirstIterator it;
    it.m_octree = this;
    return it;
}

template <class NodePayload>
typename Octree<NodePayload>::BreadthFirstIterator
Octree<NodePayload>::BreadthFirstBegin() const
{
    BreadthFirstIterator it;
    it.m_octree = this;
    it.m_nodes.push(m_root.get());
    return it;
}

template <class NodePayload>
typename Octree<NodePayload>::BreadthFirstIterator
Octree<NodePayload>::BreadthFirstEnd() const
{
    BreadthFirstIterator it;
    it.m_octree = this;
    return it;
}

template <class NodePayload>
bool Octree<NodePayload>::HasChildren(const Node* node) const
{
    ASSERT(node);
    return node->Children[0][0][0].get() != 0;
}

template <class NodePayload>
math::AABB Octree<NodePayload>::NodeSubBounds(Node* node,
                                              char x, char y, char z)
{
    ASSERT(x == 0 || x == 1);
    ASSERT(y == 0 || y == 1);
    ASSERT(z == 0 || z == 1);

    real_t minX = node->Bounds.MinX();
    real_t minY = node->Bounds.MinY();
    real_t minZ = node->Bounds.MinZ();
    const real_t halfWidth = .5f * (node->Bounds.MaxX() - minX);
    const real_t halfHeight = .5f * (node->Bounds.MaxY() - minY);
    const real_t halfDepth = .5f * (node->Bounds.MaxZ() - minZ);
    minX += static_cast<real_t>(x) * halfWidth;
    minY += static_cast<real_t>(y) * halfHeight;
    minZ += static_cast<real_t>(z) * halfDepth;
    return math::AABB(minX, minY, minZ,
                      minX + halfWidth, minY + halfHeight, minZ + halfDepth);
}


// template <class NodePayload>
// void Octree<NodePayload>::GrowRoot(const math::AABB& toContain)
// {
//     // Don't use this yet. It's probably wrong.
//     ASSERT(false);

//     if (Contains(LookupNode(m_root).Bounds, toContain))
//         return;

//     math::AABB parentBounds;
//     Union(parentBounds, toContain, LookupNode(m_root).Bounds);

//     math::Vector3 minDiffs = LookupNode(m_root).Bounds.MinCorner() -
//             parentBounds.MinCorner();
//     math::Vector3 maxDiffs = parentBounds.MaxCorner() -
//             LookupNode(m_root).Bounds.MaxCorner();

//     real_t halfParentWidth = LookupNode(m_root).Bounds.MaxX() -
//             LookupNode(m_root).Bounds.MinX();

//     // (x, y, z) describe the octant of the current root in the new root.
//     int x = 0, y = 0, z = 0;
//     if (minDiffs[0] > maxDiffs[0])
//     {
//         x = 1;
//         parentBounds.SetMaxX(LookupNode(m_root).Bounds.MaxX());
//         parentBounds.SetMinX(parentBounds.MaxX() - 2.f * halfParentWidth);
//     }
//     else
//     {
//         parentBounds.SetMinX(LookupNode(m_root).Bounds.MinX());
//         parentBounds.SetMaxX(parentBounds.MinX() + 2.f * halfParentWidth);
//     }

//     if (minDiffs[1] > maxDiffs[1])
//     {
//         y = 1;
//         parentBounds.SetMaxY(LookupNode(m_root).Bounds.MaxY());
//         parentBounds.SetMinY(parentBounds.MaxY() - 2.f * halfParentWidth);
//     }
//     else
//     {
//         parentBounds.SetMinY(LookupNode(m_root).Bounds.MinY());
//         parentBounds.SetMaxY(parentBounds.MinY() + 2.f * halfParentWidth);
//     }

//     if (minDiffs[2] > maxDiffs[2])
//     {
//         z = 1;
//         parentBounds.SetMaxZ(LookupNode(m_root).Bounds.MaxZ());
//         parentBounds.SetMinZ(parentBounds.MaxZ() - 2.f * halfParentWidth);
//     }
//     else
//     {
//         parentBounds.SetMinZ(LookupNode(m_root).Bounds.MinZ());
//         parentBounds.SetMaxZ(parentBounds.MinZ() + 2.f * halfParentWidth);
//     }

//     Node* parentNodeID = NextFreeNode();

//     LookupNode(parentNodeID).Bounds = parentBounds;
//     LookupNode(parentNodeID).TotalObjects = LookupNode(m_root).TotalObjects;

//     AddChildrenToNode(parentNodeID);
//     AddFreeNode(LookupNode(parentNodeID).Children[x][y][z]);
//     LookupNode(parentNodeID).Children[x][y][z] = m_root;
//     LookupNode(parentNodeID).Parent = 0;

//     LookupNode(m_root).Parent = parentNodeID;
//     m_root = parentNodeID;

//     // Try again - we may end up adding another level if we weren't able to
//     // fit the object in.
//     GrowRoot(toContain);
// }

template <class NodePayload>
void Octree<NodePayload>::AddChildrenToNode(Node* node)
{
    ASSERT(!HasChildren(node));
    // Create new children.
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            for (int k = 0; k < 2; ++k)
            {
                Node* child = new Node;
                child->Bounds = NodeSubBounds(node, i, j, k);
                child->m_parent = node;
                node->Children[i][j][k].reset(child);
            }
        }
    }
}

template <class NodePayload>
void Octree<NodePayload>::RemoveChildrenFromNode(Node* node)
{
    if (HasChildren(node))
    {
        for (int i = 0; i < 2; ++i)
        {
            for (int j = 0; j < 2; ++j)
            {
                for (int k = 0; k < 2; ++k)
                {
                    node->Children[i][j][k].reset(0);
                }
            }
        }
    }
}

} // namespace romulus
