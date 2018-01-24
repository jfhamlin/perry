#ifndef _CORESERVICE_H_
#define _CORESERVICE_H_

#include "Core/Types.h"
#include "Core/BufferedMessageListener.h"
#include "Core/MessageRouter.h"
#include "Core/Object.h"
#include <boost/shared_ptr.hpp>

namespace romulus
{
DECLARE_MESSAGE_TYPE(TerminateService);

class Service;
typedef boost::shared_ptr<Service> ServicePtr;

//! Services are the high level components of a romulus program.
//! They are executed by a Kernel object.  Services communicate via a
//! message passing architecture.  See MessageRouter and Message.
//! To implement a Service, a derived class must implement the Tick,
//! Terminate, and the protected Initialize methods.  The protected
//! Initialize is invoked by the public Initialize, which performs
//! some basic administrative function.
class Service : public BufferedMessageListener
{
public:

    Service(): m_messageRouter(0) { }
    virtual ~Service();

    //! Update the process.
    //! \param deltaTime - Time, in seconds, since the last update.
    virtual void Tick(real_t deltaTime) = 0;

    //! Initialize the service.
    //! \return True if the service initialized properly.
    void Initialize(MessageRouter* router);

    //! Terminates the process.
    virtual void Terminate() = 0;

protected:

    virtual void Initialize() = 0;

    void SendMessage(MessageRouter::groupid_t targetGroup, MessagePtr message);
    virtual void OnMessage(MessagePtr message) { }

    MessageRouter* m_messageRouter;
};
}

#endif // _CORESERVICE_H_

