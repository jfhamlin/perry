#ifndef _KERNEL_H_
#define _KERNEL_H_

#include "Core/Message.h"
#include "Core/Service.h"
#include "Platform/WindowService.h"
#include "Render/RenderService.h"
#include "Utility/Log.h"
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <vector>

namespace romulus
{
DECLARE_MESSAGE_TYPE(TerminateProgram);

//! Controls program execution.
//! Kernel is responsible for managing systems running on, potentially,
//! multiple threads.
//! Note that Kernel only terminates the program upon a TerminateProgram
//! message.
class Kernel : public IMessageListener
{
public:

    //! Kernel constructor.
    //! \param threadCount - The number of threads to allow.
    Kernel(int threadCount = 2);
    ~Kernel();

    inline platform::WindowService* WindowService() const
    { return static_cast<platform::WindowService*>(m_windowService.get()); }

    inline render::RenderService* RenderService() const
    { return static_cast<render::RenderService*>(m_renderService.get()); }

    //! Register the window service for execution. The kernel will automatically
    //! assign it a thread.
    //! \param windowService - The window service to register. The kernel takes
    //!                        ownership of the object.
    void RegisterWindowService(platform::WindowService* windowService);

    //! Register the render service for execution. The kernel will automatically
    //! assign it a thread.
    //! \param renderService - The render service to register. The kernel takes
    //!                        ownership of the object.
    void RegisterRenderService(render::RenderService* renderService);

    //! Register a generic service for execution.
    //! \param service - The service to register. The kernel takes ownership of
    //!                  the object. It may not be one of the special services
    //!                  registered in the above methods.
    //! \param thread - The thread on which to execute the process.
    void RegisterService(Service* service,
                         MessageRouter::groupid_t messageGroup, int thread = 0);

    //! Start program execution.
    void Run();

    //! Retrieve the local message router.
    inline MessageRouter* Router() { return &m_router; }

private:

    typedef std::vector<ServicePtr> ServiceList;
    struct ThreadInfo
    {
        ThreadInfo(int index): Index(index), Terminate(false) { }

        boost::shared_ptr<boost::thread> Thread;
        int Index;
        ServiceList Services;
        bool Terminate;
        bool Live;
        MessageRouter* Router;
        romulus::Log* Log;
    };
    typedef std::vector<ThreadInfo> ThreadList;

    class ThreadMain
    {
    public:

        ThreadInfo* Info;

        ThreadMain(ThreadInfo* info);

        void operator()() const;
    };

    virtual void OnMessage(MessagePtr message);

    //! Special services of which the Kernel is aware.
    ServicePtr m_windowService;
    ServicePtr m_renderService;

    Log m_log;
    MessageRouter m_router;
    ThreadList m_threads;
    bool m_run;
};
}

#endif // _KERNEL_H_

