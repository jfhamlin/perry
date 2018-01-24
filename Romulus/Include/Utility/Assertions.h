#ifndef _ASSERTIONS_H_
#define _ASSERTIONS_H_

#include "Platform/Platform.h"
#include <boost/static_assert.hpp>
#include <sstream>

//! \file Assertions.h
//! Contains definitions for run-time and compile-time assertions.

//! #define to conform to Romulus coding conventions for macros.
#ifdef ENABLE_ASSERTIONS

#define ASSERT(exp) \
    if (!(exp)) \
        romulus::platform::HandleAssertion(#exp, __FILE__, __LINE__);

#define ASSERT_EQ(x, y)                                                 \
    if (!(x == y))                                                      \
    {                                                                   \
        std::stringstream ss;                                           \
        ss << (x) << " == " << (y) << "\n";                             \
        romulus::platform::HandleAssertion(ss.str().c_str(),            \
                                           __FILE__, __LINE__);         \
    }

#define ASSERT_EQ_EPSILON(x, y, eps)                                    \
    if (((x) - (y)) > (eps) || ((x) - (y)) < -(eps))                    \
    {                                                                   \
        std::stringstream ss;                                           \
        ss << (x) << " == " << (y) <<                                   \
                "  (epsilon = " << (eps) << ")\n";                      \
        romulus::platform::HandleAssertion(ss.str().c_str(),            \
                                           __FILE__, __LINE__);         \
    }

#define ASSERT_GT(x, y)                                                 \
    if ((x) <= (y))                                                     \
    {                                                                   \
        std::stringstream ss;                                           \
        ss << (x) << " > " << (y) << "\n";                              \
        romulus::platform::HandleAssertion(ss.str().c_str(),            \
                                           __FILE__, __LINE__);         \
    }

#define ASSERT_LT(x, y)                                                 \
    if ((x) >= (y))                                                     \
    {                                                                   \
        std::stringstream ss;                                           \
        ss << (x) << " < " << (y) << "\n";                              \
        romulus::platform::HandleAssertion(ss.str().c_str(),            \
                                           __FILE__, __LINE__);         \
    }

#else /* Assertions off */

#define ASSERT(exp) do { (void)sizeof(exp); } while (false)
#define ASSERT_EQ(x, y)                                         \
    do { (void)sizeof(x);(void)sizeof(y); } while (false)
#define ASSERT_EQ_EPSILON(x, y, eps)                                    \
    do { (void)sizeof(x);(void)sizeof(y);(void)sizeof(eps); }       \
    while (false)
#define ASSERT_GT(x, y)                                         \
    do { (void)sizeof(x);(void)sizeof(y); } while (false)
#define ASSERT_LT(x, y)                                         \
    do { (void)sizeof(x);(void)sizeof(y); } while (false)

#endif

//! #define to keep assertion style consistent.
#define STATIC_ASSERT(exp) BOOST_STATIC_ASSERT(exp)

#endif // _ASSERTIONS_H_
