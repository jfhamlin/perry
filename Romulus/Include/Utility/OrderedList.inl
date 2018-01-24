#ifndef _ORDEREDLIST_INL_
#define _ORDEREDLIST_INL_

#include "Math/Utilities.h"
#include "Utility/Assertions.h"

namespace romulus
{

template <typename T>
void OrderedList<T>::Erase(Iterator pos)
{
    if (pos != End())
        EraseImpl(m_recordMap.find(*pos)->second.first);
}

template <typename T>
void OrderedList<T>::Erase(const T& record)
{
    typename RecordMap::iterator it = m_recordMap.find(record);
    if (it != m_recordMap.end())
        EraseImpl(it->second.first);
}

template <typename T>
bool OrderedList<T>::Order(const Iterator before,
                           const Iterator after) const
{
    if (before == End() || after == End())
        return false;
    LabelType beforeLabel = m_recordMap.find(*before)->second.second;
    LabelType afterLabel = m_recordMap.find(*after)->second.second;
    return LabelOrder(beforeLabel, afterLabel);
}

template <typename T>
typename OrderedList<T>::Iterator OrderedList<T>::Find(const T& record) const
{
    typename RecordMap::const_iterator it = m_recordMap.find(record);
    if (it == m_recordMap.end())
        return End();
    return it->second.first;
}

template <typename T>
void OrderedList<T>::AssertInvariants() const
{
    ASSERT(Size() == m_recordList.size());
    ASSERT(m_inverseThresholdConstant < 1.0f);
    ASSERT(m_inverseThresholdConstant > 0.5f);
}

template <typename T>
typename OrderedList<T>::NonConstIterator OrderedList<T>::GetNonConstIter(
        const Iterator pos)
{
    if (pos == End())
        return m_recordList.end();
    return m_recordMap.find(*pos)->second.first;
}

template <typename T>
typename OrderedList<T>::LabelType OrderedList<T>::GetLabel(const Iterator pos)
        const
{
    if (pos == End())
        return m_baseLabel;
    ASSERT(Find(*pos) != End());
    return m_recordMap.find(*pos)->second.second;
}

template <typename T>
void OrderedList<T>::SetLabel(const Iterator pos, LabelType label)
{
    if (pos == End())
    {
        m_baseLabel = label;
    }
    else
    {
        ASSERT(Find(*pos) != End());
        m_recordMap.find(*pos)->second.second = label;
    }
}

template <typename T>
bool OrderedList<T>::LabelOrder(LabelType before, LabelType after) const
{
    return (before - m_baseLabel) < (after - m_baseLabel);
}

template <typename T>
void OrderedList<T>::EraseImpl(NonConstIterator pos)
{
    if (pos != End())
    {
        typename RecordMap::iterator mapIter = m_recordMap.find(*pos);
        ASSERT(mapIter->second.first == pos);
        m_recordMap.erase(mapIter);
        m_recordList.erase(pos);
        --m_size;
    }
    SetThresholdConstant();

    AssertInvariants();
}

template <typename T>
typename OrderedList<T>::Iterator OrderedList<T>::InsertImpl(Iterator pos,
                                                             const T& record,
                                                             NewLabelPolicy p)
{
    if (pos != End() && *pos == record)
        return pos;

    Iterator prevRecord = Find(record);
    if (prevRecord != End())
        Erase(prevRecord);

    Iterator prevPos = pos;
    DecrementCircular(prevPos);

    LabelType prevPosLabel = GetLabel(prevPos);
    LabelType posLabel = GetLabel(pos);

    if (posLabel - prevPosLabel == 1)
    {
        Relabel(prevPos);
        posLabel = GetLabel(pos);
        prevPosLabel = GetLabel(prevPos);
    }

    // We intialize this variable because gcc is bad at inference.
    LabelType newLabel = 0;

    switch (p)
    {
        case NewLabelPolicy_Begin:
            newLabel = prevPosLabel + 1;
            break;
        case NewLabelPolicy_Average:
            // Assign the new record the average of the two adjacent records'
            // labels.
            if (Size() == 0)
                newLabel = posLabel + sm_maxLabel / 2;
            else
                newLabel = prevPosLabel + (posLabel - prevPosLabel) / 2;
            break;
        case NewLabelPolicy_End:
            newLabel = posLabel - 1;
            break;
    }

    ASSERT(LabelOrder(prevPosLabel, newLabel));
    ASSERT(pos == End() || LabelOrder(newLabel, posLabel));
    NonConstIterator nonConstPos = GetNonConstIter(pos);
    NonConstIterator newRecordIter = m_recordList.insert(nonConstPos, record);
    m_recordMap[record] = RecordIterLabelPair(newRecordIter, newLabel);

    ++m_size;

    SetThresholdConstant();

    AssertInvariants();

    return newRecordIter;
}

template <typename T>
void OrderedList<T>::Relabel(Iterator pos)
{
    // The new element will be inserted immediately after pos, so we start by
    // growing the label range after the label of pos.
    Iterator endPos = pos;
    IncrementCircular(endPos);
    LabelType minLabel = GetLabel(pos), maxLabel = GetLabel(endPos);
    ASSERT(minLabel == maxLabel - 1);

    uint_t numRecords = 2;
    LabelType numLabels = 2;
    float density = 1.0f;
    float threshold = m_inverseThresholdConstant;

    while (density > threshold)
    {
        minLabel -= numLabels / 2;
        maxLabel += numLabels / 2;

        // We want to double the range of labels we're looking at,
        // so look at 1/2 numLabels to the right of endPos and
        // 1/2 numLabels to the left of pos and include any records found.
        while (LabelOrder(minLabel - 1, GetLabel(DecrementCircular(pos))))
            ++numRecords;
        // We go one too far in the loop (past minLabel). Go back.
        IncrementCircular(pos);

        while (LabelOrder(GetLabel(IncrementCircular(endPos)), maxLabel + 1))
            ++numRecords;
        // We go one too far in the loop (past maxLabel). Go back.
        DecrementCircular(endPos);

        numLabels *= 2;
        density = static_cast<float>(numRecords) /
                static_cast<float>(numLabels);
        threshold *= m_inverseThresholdConstant;
    }

    const LabelType offset = numLabels / (numRecords - 1);

    SetLabel(pos, minLabel);
    SetLabel(endPos, maxLabel);

    // Now that we have a range, do the relabeling.
    uint_t i;
    for (i = 1, IncrementCircular(pos); pos != endPos; IncrementCircular(pos))
    {
        SetLabel(pos, minLabel + i * offset);
        ++i;
    }
}

template <typename T>
typename OrderedList<T>::Iterator&
OrderedList<T>::DecrementCircular(Iterator& it) const
{
    if (Size())
    {
        if (it == Begin())
            it = End();
        else
            --it;
    }
    return it;
}

template <typename T>
typename OrderedList<T>::Iterator&
OrderedList<T>::IncrementCircular(Iterator& it) const
{
    if (Size())
    {
        if (it == End())
            it = Begin();
        else
            ++it;
    }
    return it;
}

} // namespace romulus

#endif // _ORDEREDLIST_INL_
