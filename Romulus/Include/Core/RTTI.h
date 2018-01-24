#ifndef _RTTI_H_
#define _RTTI_H_

#include "Core/Types.h"
#include <string>

namespace romulus
{
//! Type information.
class RTTI
{
public:

    //! The parent type's type information.
    const RTTI*  Parent;
    //! The type name.
    const char*  Name;
    //! The type size, in bytes.
    const uint_t Size;

    //! A runtime-dependent unique type ID.
    const int ID;

    inline RTTI(const char* name, uint_t size, const RTTI* parent):
        Parent(parent), Name(name), Size(size), ID(sm_nextID++)
    {
    }


    inline bool operator ==(const RTTI& typeInfo) const
    { return typeInfo.Name == Name; }

    inline bool operator !=(const RTTI& typeInfo) const
    { return typeInfo.Name != Name; }

private:

    static int sm_nextID;
};

#define DECLARE_BASE_RTTI \
    public: \
        static const ::romulus::RTTI TypeInformation;                       \
        virtual const ::romulus::RTTI& Type() const { return TypeInformation; } \
        virtual bool IsBaseType() const { return Type().Parent == 0; }

#define DECLARE_RTTI(Parent) \
    public: \
        typedef Parent ParentType; \
        static const ::romulus::RTTI TypeInformation;                       \
        virtual const ::romulus::RTTI& Type() const { return TypeInformation; }

#define IMPLEMENT_BASE_RTTI(Name) \
    const ::romulus::RTTI Name::TypeInformation(#Name, sizeof(Name), 0)

#define IMPLEMENT_RTTI(Name) \
    const ::romulus::RTTI Name::TypeInformation(#Name, sizeof(Name),    \
                                              &Name::ParentType::TypeInformation)

#define IMPLEMENT_RTTI_TEMPLATE(Name) \
    template <typename T> \
    IMPLEMENT_RTTI(Name<T>)

template <typename T, typename U>
bool IsExactType(const U& obj)
{
    return obj.Type() == T::TypeInformation;
}

template <typename T, typename U>
bool IsSubtype(const U& obj)
{
    const RTTI* rtti = obj.Type().Parent;

    while (rtti)
    {
        if (*rtti == T::TypeInformation)
            return true;

        rtti = rtti->Parent;
    }

    return false;
}

template <typename T, typename U>
bool IsType(const U& obj)
{
    return IsExactType<T>(obj) || IsSubtype<T>(obj);
}

}

#endif // _RTTI_H_

