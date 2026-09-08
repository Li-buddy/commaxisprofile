#pragma once

// ---------------------------------------------------------------------------
//
//! @file       mutex_helper.h
//!
//! @author     DC-AE/ESW2
//!
//! @copyright  Bosch Rexroth Corporation http://www.boschrexroth.com
//!
//! @version    1.0.0
//!
//! @date       2019
//
// ---------------------------------------------------------------------------
//
//! This file is part of the axisprofile library and implements a helper class for usage of mutex.
//
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include "common/aox/inherit_recursive_mutex.h"

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm
{
  namespace axisprofile
  {
    // ---------------------------------------------------------------------------
    // Declaration of symbolic constants
    // ---------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    // Declaration of macros
    // ---------------------------------------------------------------------------
    //! This macro prints the given parameter as string.
    #define _CLASS_NAME_(x)   (           # x )
    //! This macro maps the given parameter to a macro that prints it as string.
    #define CLASS_NAME(x)     (_CLASS_NAME_(x))


    // ---------------------------------------------------------------------------
    // Declaration of data types
    // ---------------------------------------------------------------------------


    // ---------------------------------------------------------------------------
    // Declaration of classes and structures
    // ---------------------------------------------------------------------------

    //! @class MutexHelper
    //! This is a helper class for usage of a mutex in the axisprofile.
    class MutexHelper
    {
    public: // methods
      MutexHelper();
      virtual ~MutexHelper() noexcept;

      //! @brief This method returns the class name.
      //!
      //! @return                      Return class name.
      static const char* getClassName(void) { return (CLASS_NAME(MutexHelper)); }

      //! @brief Locks the mutex and waits until the lock succeeds.
      //! @return Returns true if the lock is taken.
      bool lock();

      //! @brief Unlocks the mutex and waits until the unlock succeeds.
      //! @return Returns true if the lock is freed.
      bool unlock();

      MutexHelper& operator=(const MutexHelper& other)
      {
        if (this != &other)
        {
          // Release existing resources, if needed
          // Perform proper resource copying, e.g., creating new mutex
          this->m_mutex = other.m_mutex;
        }
        return *this;
      }

    private:
      MutexHelper(const MutexHelper&) = delete;

    private:

      common::aox::InheritMutex* m_mutex; //!< mutex for locking
    };

    //! @class CmpMutexLockHelper
    //! use this class as local variable to aquire a mutex in a method and release it, when returning in the method
    class CmpMutexLockHelper
    {
    public:
      //! @brief constructor
      //! @param[in]   poMutex    pointer to mutex
      //! @return Returns true if the lock is freed.
      explicit CmpMutexLockHelper(MutexHelper* poMutex) : c_poMutex(poMutex)
      {
        c_poMutex->lock();
      }

      virtual ~CmpMutexLockHelper()
      {
        c_poMutex->unlock();
      }

    private:
      MutexHelper* c_poMutex; //! mutex helper for locking
    };
  }
}
