#ifndef _ORDEREDLIST_H_
#define _ORDEREDLIST_H_

//! \file OrderedList.h
//! Contains the class definition for the OrderedList container.

#include "Core/Types.h"
#include "Utility/Common.h"
#include <boost/functional/hash.hpp>
#include <list>
#include <map>

namespace romulus
{

//! A class that maintains an order among records, supporting insertion,
//! deletion, and constant-time order queries. Only one of any value may be
//! added to the list. Requirements are as for the standard library's
//! containers: objects must be default and copy and support the assignment
//! operator. Also, the values must be hashable. Public iterators for this
//! class are all const.
template <typename T>
class OrderedList
{
PROHIBIT_COPYING(OrderedList);
public:
    typedef typename std::list<T>::const_iterator Iterator;
    typedef typename std::list<T>::const_reverse_iterator ReverseIterator;

    OrderedList(): m_size(0) { SetThresholdConstant(); }

    //! Insert a new record in the list before position pos in this list.
    //! Behavior is undefined if pos is not an iterator from this list.
    //! If an element already exists that is equal to record, the old element
    //! will be removed and the new one inserted.
    //! \param pos - an Iterator from this list.
    //! \param record - a record to insert in the list.
    //! \return An Iterator to the new element.
    inline Iterator Insert(Iterator pos, const T& record)
    {
        return InsertImpl(pos, record, NewLabelPolicy_Average);
    }

    //! Removes the element at pos from the list. Behavior is undefined if
    //! pos is not an iterator of this list. If pos == End(), no change is
    //! made to the list.
    //! \param pos - an Iterator of this list.
    void Erase(Iterator pos);

    //! Removes any element with value record from the list.
    void Erase(const T& record);

    //! Query whether one record precedes another in the ordering in constant
    //! time. If either parameter is not an iterator of this list, behavior is
    //! undefined.
    //! \return Returns true if before comes before after in the ordering. If
    //!         the elements are identical, false is returned.
    bool Order(const Iterator before, const Iterator after) const;

    //! Query whether one record precedes another in the ordering in constant
    //! time.
    //! \return Returns true if before comes before after in the ordering. If
    //!         the elements are identical, or if one or the other value is not
    //!         an element of the list, false is returned.
    bool Order(const T& before, const T& after) const
    {
        return Order(Find(before), Find(after));
    }

    //! Returns an iterator to a record in the list.
    //! \param record - the value of the element to look up.
    //! \return Returns an iterator pointing to the element of the list
    //!         containing record if record is found, or End() if not.
    Iterator Find(const T& record) const;

    //! Returns an iterator to the beginning of the list.
    inline Iterator Begin() const { return m_recordList.begin(); }

    //! Returns an iterator to the end of the list.
    inline Iterator End() const { return m_recordList.end(); }

    //! Returns an iterator to the first element at the back of the list.
    ReverseIterator RBegin() const { return m_recordList.rbegin(); }

    //! Returns an iterator to the reverse end of the list.
    ReverseIterator REnd() const { return m_recordList.rend(); }

    //! Returns the number of elements held by the list.
    inline uint_t Size() const { return m_size; }

    //! If the list is empty, returns true. Returns false otherwise.
    inline bool Empty() const { return Size() == 0; }

    //! Returns a reference to the value of the element at the front of the
    //! list.
    inline const T& Front() const { return m_recordList.front(); }

    //! Returns a reference to the value of the element at the back of the
    //! list.
    inline const T& Back() const { return m_recordList.back(); }

    //! Inserts an element at the front of the list.
    inline void PushFront(const T& record)
    {
        InsertImpl(Begin(), record, NewLabelPolicy_Begin);
    }

    //! Inserts an element at the back of the list.
    inline void PushBack(const T& record)
    {
        InsertImpl(End(), record, NewLabelPolicy_End);
    }

    //! Removes the element at the front of the list.
    inline void PopFront()
    {
        EraseImpl(m_recordList.begin());
    }

    //! Removes the element at the back of the list.
    inline void PopBack()
    {
        EraseImpl(m_recordMap.find(Back())->second.first);
    }

    //! Removes all elements and sets the size to zero.
    inline void Clear()
    {
        m_recordMap.clear();
        m_recordList.clear();
        m_size = 0;
        SetThresholdConstant();
    }

private:

    enum NewLabelPolicy
    {
        //! Assign a new label that is at the very start of the possible
        //! insertion range.
        NewLabelPolicy_Begin,
        //! Assign a new label that is in the middle of the insertion range.
        NewLabelPolicy_Average,
        //! Assign a new label that is at the very end of the possible
        //! insertion range.
        NewLabelPolicy_End
    };

    typedef T RecordType;
    typedef uint_t LabelType;
    typedef std::list<RecordType> RecordList;
    typedef typename std::list<RecordType>::iterator NonConstIterator;
    typedef std::pair<NonConstIterator, LabelType> RecordIterLabelPair;
    typedef std::map<RecordType, RecordIterLabelPair> RecordMap;

    void AssertInvariants() const;

    //! Decrements an Iterator, wrapping around to the end of the container if
    //! necessary.
    //! \return A reference to the iterator.
    Iterator& DecrementCircular(Iterator& it) const;

    //! Increments an Iterator, wrapping around to the beginning of the
    //! container if necessary.
    //! \return A reference to the iterator.
    Iterator& IncrementCircular(Iterator& it) const;

    //! Returns a non-const iterator to the same element as pos for internal
    //! use.
    NonConstIterator GetNonConstIter(const Iterator pos);

    //! Gets the label for the element at pos.
    LabelType GetLabel(const Iterator pos) const;

    //! Sets the label for the element at pos.
    void SetLabel(const Iterator pos, LabelType label);

    //! Returns true if after comes after before in the ordering.
    bool LabelOrder(LabelType before, LabelType after) const;

    //! Erases the element at pos.
    void EraseImpl(NonConstIterator pos);

    //! Inserts an element with value record at pos, using the new label policy
    //! p. If an element with value record already exists, it is deleted.
    //! \param pos - the iterator before which the new element is to go.
    //! \param record - the new record to insert as an element.
    //! \param p - the NewLabelPolicy insertion policy. Certain insertion
    //!            methods indicate a certain type of use and so a more
    //!            relevant policy.
    //! \return Returns an iterator to the new element.
    Iterator InsertImpl(Iterator pos, const T& record, NewLabelPolicy p);

    //! Relabel records in the smallest enclosing sublist of pos that is
    //! not 'overflowing' (i.e. is less dense than the density threshold
    //! for that size sublist)
    void Relabel(Iterator pos);

    //! Set the threshold constant to an optimal value for the current Size().
    //! TODO: implement a variable constant.
    void SetThresholdConstant() { m_inverseThresholdConstant = 1.0f / 1.4f; };

    float m_inverseThresholdConstant;

    static const LabelType sm_maxLabel = -1;

    LabelType m_baseLabel;

    RecordMap m_recordMap;

    RecordList m_recordList;

    //! A size member is kept to avoid using std::list's linear time size().
    uint_t m_size;
};

}

#include "Utility/OrderedList.inl"

#endif // _ORDEREDLIST_H_
