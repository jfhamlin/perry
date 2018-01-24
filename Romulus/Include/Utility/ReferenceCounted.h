#ifndef _REFERENCECOUNTED_H_
#define _REFERENCECOUNTED_H_

//! \file ReferenceCounted.h
//! Contains the declaration and implementation of the ReferenceCounted type.

namespace romulus
{
    //! Reference counted base class.
    //! Any class which is to be reference counted should derive from
    //! this class and use the ReferencePointer type.
    class ReferenceCounted
    {
    public:

        typedef unsigned long referencecount_t;

        void AddReference();
        void SubReference();

    protected:

        ReferenceCounted();
        virtual ~ReferenceCounted() { }

        //! Called when no one is holding a reference to this object.
        virtual void OnDeath();

    private:

        referencecount_t m_referenceCount;
    };

    inline ReferenceCounted::ReferenceCounted():
    m_referenceCount(0)
    {
    }

    inline void ReferenceCounted::OnDeath()	
    {
        delete this;
    }

    inline void ReferenceCounted::AddReference()
    {
        ++m_referenceCount;
    }

    inline void ReferenceCounted::SubReference()
    {
        if (--m_referenceCount == 0)
            OnDeath();
    }
}

#endif // _REFERENCECOUNTED_H_
