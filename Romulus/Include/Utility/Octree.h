#ifndef _OCTREE_H_
#define _OCTREE_H_

#include "Math/Bounds/BoundingVolumes.h"
#include <boost/scoped_ptr.hpp>
#include <stack>
#include <queue>

namespace romulus
{

template <class NodePayload>
class Octree
{
public:

    Octree();
    Octree(const math::AABB& rootBounds);

    //! Class representing a node of the Octree.
    class Node : public NodePayload
    {
    public:
        inline const math::AABB& BoundingBox() const;
        inline Node* Parent() const;
        inline Node* Child(uint_t childIndex) const;
        inline Node* Child(uint_t x, uint_t y, uint_t z) const;

    private:
        friend class ::std::vector<Node>;
        friend class Octree;
        friend void boost::checked_delete<Node>(Node*);

        typedef boost::scoped_ptr<Node> ChildPtr;

        inline Node();
        ~Node() { }

        //! The region bounded by this node.
        math::AABB Bounds;
        //! The child nodes of this node. An invariant is that either all
        //! children are null, or all are valid.
        ChildPtr Children[2][2][2];
        //! The parent node of this node.
        Node* m_parent;
    };

    template <template <typename, typename> class TraversalContainer>
    class Iterator
    {
    public:
        inline Iterator(): m_octree(0) { }
        inline Iterator& operator++();
        inline Node& operator*() const;
        inline bool operator==(const Iterator<TraversalContainer>& it) const;
        inline bool operator!=(const Iterator<TraversalContainer>& it) const;

    private:
        friend class Octree;

        inline Node* CurrentNode(const std::stack<Node*>& ids) const;
        inline Node* CurrentNode(const std::queue<Node*>& ids) const;

        const Octree<NodePayload>* m_octree;
        TraversalContainer<Node*, std::deque<Node*> > m_nodes;
    };

    typedef Iterator<std::stack> DepthFirstIterator;
    typedef Iterator<std::queue> BreadthFirstIterator;

    DepthFirstIterator DepthFirstBegin() const;
    DepthFirstIterator DepthFirstEnd() const;

    BreadthFirstIterator BreadthFirstBegin() const;
    BreadthFirstIterator BreadthFirstEnd() const;

    //! \return the root of the Octree.
    inline Node* RootNode() const { return m_root.get(); }

    //! \return true if the node has non-null children.
    bool HasChildren(const Node* node) const;

    //! Grow the octree upwards by adding a new root (possibly multiple times)
    //!  above the current one.
    //! \param toContain - a bounding box which must be contained by the new
    //!                    root. If it is already contained by the current root
    //!                    then the octree is not modified.
    //void GrowRoot(const math::AABB& toContain);

    //! Give a childless node children.
    void AddChildrenToNode(Node* node);

    //! Remove all of a node's children.
    void RemoveChildrenFromNode(Node* node);

protected:

    //! Return an AABB bounding the octant given by (x, y, z) of the given node.
    //! This function should be used if the given node does not yet have a child
    //! bounding the given octant, otherwise constructing the AABB is redundant.
    //! \param node - the node whose octant is returned.
    //! \param x - in [0, 1], the octant towards neg. x if 0, pos. x if 1.
    //! \param y - in [0, 1], the octant towards neg. y if 0, pos. y if 1.
    //! \param z - in [0, 1], the octant towards neg. z if 0, pos. z if 1.
    //! \return The AABB of the octant of node specified by (x, y, z).
    math::AABB NodeSubBounds(Node* node, char x, char y, char z);

    boost::scoped_ptr<Node> m_root;
};

} // namespace romulus

#include "Utility/Octree.inl"

#endif // _OCTREE_H_
