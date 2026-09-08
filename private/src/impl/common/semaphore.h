//! this class has to be removed, when we have an AOX!!!
//! this is a copy from https://stackoverflow.com/questions/4792449/c0x-has-no-semaphores-how-to-synchronize-threads
//! OSS is not okay!
#pragma once


#include "common/aox/inherit_recursive_mutex.h"
#include <condition_variable>

namespace comm
{
  namespace axisprofile
  {

    //! @brief simple semaphore implementation (to be removed, when AOX is available)
    class Semaphore
    {
    private:
      common::aox::InheritMutex m_mutex;     //!< mutex for sync
      std::condition_variable m_condition;   //!< condition for wakeup calls
      unsigned long m_count = 0;             //!< counter of the semaphore, initialized as locked.

    public:
      virtual ~Semaphore() noexcept = default;

      //! @brief  notify the semaphore (equal to sem_give)
      void notify()
      {
        std::lock_guard<decltype(m_mutex)> lock(m_mutex);
        ++m_count;
        m_condition.notify_one();
      }

      //! @brief  wait on semaphore (equal to sem_take)
      void wait()
      {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (!m_count) // Handle spurious wake-ups.
        {
          m_condition.wait(lock);
        }
        --m_count;
      }

      //! @brief try to wait on semaphore (similar to sem_take, when semaphore is available)
      //! @return true, when the semaphore was available (and is not decremented by one)
      bool tryWait()
      {
        std::lock_guard<decltype(m_mutex)> lock(m_mutex);
        if (m_count)
        {
          --m_count;
          return true;
        }
        return false;
      }

      //! @brief reset the semaphore counter
      void reset()
      {
        m_count = 0;
      }
    };
  }
}
