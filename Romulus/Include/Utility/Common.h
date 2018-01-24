#ifndef _UTILITYCOMMON_H_
#define _UTILITYCOMMON_H_

//! \file Contains some commonly useful macros to simplify some C++ idioms.

//! Make a class' copy constructor and assignment operator private, prohibiting
//! copying of objects of that class. The macro specifies private access, so
//! use of it should take this into account when declaring names beneath it.
#define PROHIBIT_COPYING(class_name) \
private: \
class_name(const class_name &); \
void operator=(const class_name &)

#endif // _UTILITYCOMMON_H_
