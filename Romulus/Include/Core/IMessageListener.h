#ifndef _IMESSAGELISTENER_H_
#define _IMESSAGELISTENER_H_

#include "Core/Message.h"

namespace romulus
{
//! Receives and processes Message objects.
class IMessageListener
{
public:

    IMessageListener() { }
    virtual ~IMessageListener() { }

    //! Handle a message.
    virtual void HandleMessage(MessagePtr message)
    {
        OnMessage(message);
    }

private:

    virtual void OnMessage(MessagePtr message) = 0;
};
}

#endif // _IMESSAGELISTENER_H_
