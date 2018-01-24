#ifndef _REFERENCEPOINTER_H_
#define _REFERENCEPOINTER_H_

namespace romulus
{
    template <class T>
    class ReferencePointer
    {
    public:

        ReferencePointer();
        ReferencePointer(T* object);
        ReferencePointer(const ReferencePointer& ptr);
        ~ReferencePointer();

        inline operator T*() const { return m_object; }

        ReferencePointer& operator =(T* object);
        ReferencePointer& operator =(const ReferencePointer& ptr);

        inline T* operator ->() const { return m_object; }
        inline T& operator *() const { return *m_object; }

    private:

        T* m_object;
    };

    template <class T>
    inline ReferencePointer<T>::ReferencePointer():
    m_object(0)
    {		
    }

    template <class T>
    inline ReferencePointer<T>::ReferencePointer(T* object):
    m_object(object)
    {
        if (m_object)
            m_object->AddReference();
    }

    template <class T>
    inline ReferencePointer<T>::ReferencePointer(const ReferencePointer<T>& ptr):
    m_object(ptr.m_object)
    {
        if (m_object)
            m_object->AddReference();
    }

    template <class T>
    inline ReferencePointer<T>::~ReferencePointer()
    {
        if (m_object)
            m_object->SubReference();
    }

    template <class T>
    inline ReferencePointer<T>& ReferencePointer<T>::operator =(T* object)
    {
        if (object == m_object)
            return *this;

        if (m_object)
            m_object->SubReference();
        m_object = object;
        if (m_object)
            m_object->AddReference();
        return *this;
    }

    template <class T>
    inline ReferencePointer<T>& ReferencePointer<T>::operator =(const ReferencePointer<T>& ptr)
    {
        if (m_object == ptr.m_object)
            return *this;

        if (m_object)
            m_object->SubReference();
        m_object = ptr.m_object;
        if (m_object)
            m_object->SubReference();
        return *this;
    }	
}

#endif // _REFERENCEPOINTER_H_
