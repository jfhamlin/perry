#ifndef _MATHTYPES_H_
#define _MATHTYPES_H_

//! \file Types.h
//! Contains math constants.

#include "Core/Types.h"
#include <limits>

namespace romulus
{
namespace math
{
const real_t AcceptableMathEpsilon = std::numeric_limits<real_t>::epsilon();

const real_t Infinity = std::numeric_limits<real_t>().infinity();

const real_t Pi = 3.14159265;
}
}

#endif // _MATHTYPES_H_
