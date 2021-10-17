#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#include <atomic>
#include <sched.h>

namespace dindex
{

class Spinlock 
{
public:
    Spinlock() { m_lock.clear(); }
    Spinlock(const Spinlock&) = delete;
    ~Spinlock() = default;

    void lock() 
    {
        if (!m_lock.test_and_set(std::memory_order_acquire)) {
            return ;
        }

        while (1) {
            for (int32_t i=0; i<4; ++i) {
                if (!m_lock.test_and_set(std::memory_order_acquire)) {
                    return ;
                }
            }

            sched_yield();  // usleep(1)
        }

        return ;
    }
    
    bool try_lock() 
    {
        return !m_lock.test_and_set(std::memory_order_acquire);
    }
    
    void unlock() 
    {
        m_lock.clear(std::memory_order_release);
    }

private:
    std::atomic_flag m_lock;
};

}

#endif //_SPINLOCK_H_

