#ifndef _SCENEIBOUNDED_H_
#define _SCENEIBOUNDED_H_

namespace romulus
{
namespace math
{
//! Forward declaration of the IBoundingVolume class.
class IBoundingVolume;
}

namespace scene
{

class IBounded
{
public:

    virtual ~IBounded() { }

    virtual const math::IBoundingVolume& BoundingVolume() const = 0;
};

} // namespace scene
} // namespace romulus

#endif // _SCENEIBOUNDED_H_
