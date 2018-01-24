#ifndef _SHAREDPOINTER_H_
#define _SHAREDPOINTER_H_

#include <boost/shared_ptr.hpp>

#define DECLARE_SHARED_PTR(Class) \
  typedef boost::shared_ptr< Class > Class##Ptr

#endif // _SHAREDPOINTER_H_
