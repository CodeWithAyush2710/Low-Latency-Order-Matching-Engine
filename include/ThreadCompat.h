#ifndef THREAD_COMPAT_H
#define THREAD_COMPAT_H

#ifdef NO_STD_THREAD_SUPPORT
    // Dummy implementation for compilers without std::thread support (e.g., MinGW win32)
    struct DummyMutex {};
    struct DummyLock { DummyLock(DummyMutex&) {} };
    using MutexType = DummyMutex;
    using LockType = DummyLock;
#else
    #include <mutex>
    #include <thread>
    using MutexType = std::mutex;
    using LockType = std::lock_guard<std::mutex>;
#endif

#endif // THREAD_COMPAT_H
