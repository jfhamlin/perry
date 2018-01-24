#ifndef _RESOURCEGEOMETRYGENERATOIRS_H_
#define _RESOURCEGEOMETRYGENERATOIRS_H_

#include "Math/Constants.h"
#include <boost/shared_ptr.hpp>
#include <cmath>

namespace romulus
{

namespace render
{
class GeometryChunk;
}

boost::shared_ptr<render::GeometryChunk> CreateNewSphere(
        uint_t lonSegments, uint_t latSegments);

} // namespace romulus

#endif // _RESOURCEGEOMETRYGENERATOIRS_H_
