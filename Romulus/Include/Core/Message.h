#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include "Core/RTTI.h"
#include <boost/shared_ptr.hpp>

namespace romulus
{
#define DECLARE_MESSAGE_TYPE(Name) \
    class Name##Message : public romulus::Message { }

//! Base message type.
//! Messages are passed via a MessageRouter.
class Message
{
DECLARE_BASE_RTTI;
public:

    Message() { }
    virtual ~Message() { }
};
typedef boost::shared_ptr<Message> MessagePtr;
}

#endif // _MESSAGE_H_

