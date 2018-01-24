#ifndef _COREBUFFEREDMESSAGELISTENER_H_
#define _COREBUFFEREDMESSAGELISTENER_H_

//! \file BufferedMessageListener.h
//! Contains declaration of the buffered message listener class.

#include "Core/IMessageListener.h"
#include <boost/thread/mutex.hpp>
#include <vector>

namespace romulus
{

//! A thread safe message listener.
//! This listener accumulates incoming messages in a collection.
//! To process these messages, call PumpMessages().
class BufferedMessageListener : public IMessageListener
{
public:

    BufferedMessageListener();
    virtual ~BufferedMessageListener();

    virtual void HandleMessage(MessagePtr message);

protected:

    void PumpMessages();
    virtual void OnMessage(MessagePtr message) = 0;

private:

    typedef std::vector<MessagePtr> MessageCollection;

    MessageCollection m_messages;
    boost::mutex m_lock;
};

}

#endif // _COREBUFFEREDMESSAGELISTENER_H_
