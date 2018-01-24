#ifndef _COREMESSAGEROUTER_H_
#define _COREMESSAGEROUTER_H_

#include "Core/Types.h"
#include "Core/Message.h"
#include "Core/IMessageListener.h"
#include <vector>
#include <map>
#include <boost/thread/mutex.hpp>

namespace romulus
{
//! MessageRouter directs Message objects to various receiver groups.
class MessageRouter
{
public:

    typedef uint_t groupid_t;

    static MessageRouter DefaultRouter;

    MessageRouter();
    ~MessageRouter();

    //! Register a listener to receive messages.
    //! \param group - The message group to register with.
    //! \param listener - The message listener to register.
    void RegisterListener(groupid_t group, IMessageListener* listener);
    //! Unregister a listener from a certain group.
    //! \param group - The group from which to unregister the listener.
    //! \param listener - The listener to unregister.
    void UnregisterListener(groupid_t group, IMessageListener* listener);

    //! Unregister a listener from every group to which it subscribes.
    //! \param listener - The listener to unregister.
    void UnregisterListener(IMessageListener* listener);

    //! Send a message to a particular group.
    //! \param group - The target group.
    //! \param message - The message to send.
    void DispatchMessage(groupid_t group, MessagePtr message);

private:

    typedef std::vector<IMessageListener*> MessageListenerList;
    typedef std::map<groupid_t, MessageListenerList> MessageGroupMap;

    MessageGroupMap m_groups;

    boost::mutex m_groupsMutex;
};

const MessageRouter::groupid_t KernelMessageGroup = 0;
const MessageRouter::groupid_t WindowServiceMessageGroup = 1;
const MessageRouter::groupid_t RenderServiceMessageGroup = 2;
const MessageRouter::groupid_t MutableResourceMessageGroup = 3;

}

#endif // _COREMESSAGEROUTER_H_

