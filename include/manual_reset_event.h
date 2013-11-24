#ifndef MANUAL_RESET_EVENT_H
#define MANUAL_RESET_EVENT_H


class manual_reset_event
{
public:
    manual_reset_event(bool signaled = false)
        : signaled_(signaled)
    {
    }

    void set()
    {
        {
            std::unique_lock<std::mutex> lock(m_);
            signaled_ = true;
        }

        // Notify all because until the event is manually
        // reset, all waiters should be able to see event signalling
        cv_.notify_all();
    }

    void unset()
    {
        std::unique_lock<std::mutex> lock(m_);
        signaled_ = false;
    }


    void wait()
    {
        std::unique_lock<std::mutex> lock(m_);
        while (!signaled_)
        {
            cv_.wait(lock);
        }
    }

private:
    std::mutex m_;
    std::condition_variable cv_;
    bool signaled_;
};

#endif // MANUAL_RESET_EVENT_H
