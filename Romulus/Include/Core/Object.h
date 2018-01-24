#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "Core/RTTI.h"
#include "Core/Types.h"
#include <boost/functional/hash.hpp>
#include <string>
#include <vector>

namespace romulus
{
#define DECLARE_OBJECT_POINTER(Name) \
        class Name; \
        typedef romulus::ObjectPointer<Name> Name##Ptr

class ObjectPointerBase
{
protected:

    void Acquire(class Object* object);
    void Release(class Object* object);
};

template <class T>
class ObjectPointer : protected ObjectPointerBase
{
public:

    inline ObjectPointer(): m_object(0) { }
    inline ObjectPointer(T* object): m_object(0) { Set(object); }
    inline ObjectPointer(const ObjectPointer& ptr): m_object(0)
        { Set(ptr.m_object); }

    inline ObjectPointer& operator=(T* object)
    {
        Set(object);
        return *this;
    }
    inline ObjectPointer& operator=(const ObjectPointer& ptr)
    {
        Set(ptr.m_object);
        return *this;
    }

    inline T* operator->() const { return m_object; }

    inline T& operator*() const { return *m_object; }

    inline T* GetPointer() const { return m_object; }

    inline bool operator==(const T* object) const
    { return object == m_object; }
    inline bool operator==(const ObjectPointer& ptr) const
    { return ptr.m_object == m_object; }
    inline bool operator!=(const T* object) const
    { return object != m_object; }
    inline bool operator!=(const ObjectPointer& ptr) const
    { return ptr.m_object != m_object; }
    inline bool operator<(const ObjectPointer& ptr) const
    { return m_object < ptr.m_object; }

    friend std::size_t hash_value(const ObjectPointer<T>& p)
    {
        boost::hash<T*> hasher;
        return hasher(p.m_object);
    }

    //! Function necessary for compatibility with Boost tools such as mem_fn.
    friend T* get_pointer(const ObjectPointer<T>& p) { return p.GetPointer(); }

private:

    inline void Set(T* object)
    {
        if (m_object == object)
            return;
        if (m_object)
            Release(m_object);
        m_object = object;
        if (m_object)
            Acquire(m_object);
    }

    T* m_object;
};

DECLARE_OBJECT_POINTER(Object);
//! Basic memory managed object type.
//! Object is the base class for dynamically allocated, serializable
//! objects.
class Object
{
DECLARE_BASE_RTTI;
public:

    class GUID
    {
    public:
        bool operator==(const GUID& guid) const
        { return m_pointer == guid.m_pointer; }

        bool operator<(const GUID& guid) const
        { return m_pointer < guid.m_pointer; }

        friend std::size_t hash_value(const GUID& guid)
        {
            boost::hash<const Object*> hasher;
            return hasher(guid.m_pointer);
        }

    private:
        inline GUID(const Object* const pointer): m_pointer(pointer) { }
        inline GUID(): m_pointer(0) { }

        friend class Object;
        const Object* m_pointer;
    };
    typedef GUID guid_t;

    inline uint_t ReferenceCount() const { return m_references; }
    inline guid_t ID() const { return GUID(this); }

    //! Free memory occupied by unreferenced objects.
    static void CollectGarbage();
    //! \return A string containing memory statistics.
    static std::string MemoryStatistics();

    //! This method should be used with caution, since it circumvents Object's
    //! reference counting system. It should be used for fast conversions to
    //! derived pointer types for wrapping in ObjectPointers.
    //! \return The associated Object, or 0 if none exist.
    static Object* LookupObject(const guid_t objectID);

protected:

    Object();
    virtual ~Object();

private:

    void Acquire();
    void Release();

    friend class ObjectPointerBase;

    uint_t m_references;


    typedef std::vector<Object*> ObjectList;
    static ObjectList sm_objects;
    static ObjectList sm_deadObjects;
    static uint_t sm_liveSize;
    static uint_t sm_deadSize;
};

} // namespace romulus

#endif // _OBJECT_H_
