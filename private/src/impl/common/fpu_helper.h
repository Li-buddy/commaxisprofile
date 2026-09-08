#pragma once

/// @file
///
/// @brief This source file is part of the axisprofile library and implement the FPU error check method.
///
/// SPDX-FileCopyrightText: Copyright (c) 2024 Bosch Rexroth AG
///
/// The reproduction, distribution and utilization of this file as well as the communication of its contents to others
/// without express authorization is prohibited. Offenders will be held liable for the payment of damages. 
/// All rights reserved in the event of the grant of a patent, utility model or design.

// ---------------------------------------------------------------------------
// Include Files
// ---------------------------------------------------------------------------
#include <cstdint>
#include <cfenv>
#include <cmath>
#include <cfloat>
#include <cstdarg>

// ---------------------------------------------------------------------------
// Forward references
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// Name Spaces
// ---------------------------------------------------------------------------
namespace comm::axisprofile::fpu
{
  // ---------------------------------------------------------------------------
  // Declaration of macros
  // ---------------------------------------------------------------------------

  /// @brief this macro enables the FPU exceptions (stop debugger on invalid FPU operation)
  /// Note: FPU control word is thread local.
#ifdef WIN32
#define ENABLE_FPU_EXCEPTIONS  \
    (void)_controlfp_s(NULL, 0, _EM_ZERODIVIDE | _EM_INVALID | _EM_OVERFLOW);
#else
#define ENABLE_FPU_EXCEPTIONS  \
    (void)feenableexcept(FE_DIVBYZERO | FE_INVALID | FE_OVERFLOW);
#endif

  /// @brief this macro disables the FPU exceptions (don't stop on invalid FPU operation)
  /// Note: FPU control word is thread local.
#ifdef WIN32
#define DISABLE_FPU_EXCEPTIONS  \
    (void)_controlfp_s(NULL, 0, 0);
#else
#define DISABLE_FPU_EXCEPTIONS  \
    (void)feenableexcept(0);
#endif

  // ---------------------------------------------------------------------------
  // Declaration of data types
  // ---------------------------------------------------------------------------

  // ---------------------------------------------------------------------------
  // Declaration of symbolic constants
  // ---------------------------------------------------------------------------
  /// default numerical limit within which a floating point value is considered to be valid
  constexpr double DEFAULT_NUMERICAL_LIMIT = 1.0E15;

  // ---------------------------------------------------------------------------
  // Declaration of classes and structures
  // ---------------------------------------------------------------------------

  /// @brief check, if a double value is a valid number
  /// @param[in]  d   the double value to be tested
  /// @return  nullptr, when it is a valid number, else pointer to some error text
  inline const char* checkValid(double d)
  {
    if (std::isfinite(d))
    {
      if (d < -DEFAULT_NUMERICAL_LIMIT)
      {
        return "less than default numerical limit";
      }
      if (d > DEFAULT_NUMERICAL_LIMIT)
      {
        return "greater than default numerical limit";
      }
      return nullptr;   // number is okay
    }

    if (std::isinf(d))
    {
      return "is infinite";
    }

    if (std::isnan(d))
    {
      return "is not a number";
    }
    return "unknown error";
  }

  /// @brief check for FPU errors and returns an error code
  ///
  /// The function expects these error codes with increasing values: DivByZero, Invalid, Overflow, Unknown.
  /// Underflow and Inexact are ignored.
  ///
  /// @param[in]  okay            error code for 'no FPU error'
  /// @param[in]  firstFPUError   error code for the first FPU error
  /// @return  Error code related to current FPU error state
  template<typename RETURN_TYPE> inline RETURN_TYPE checkForFPUError(RETURN_TYPE okay, RETURN_TYPE firstFPUError)
  {
    // check for FPU exception without Underflow and Inexact
    static constexpr std::uint32_t FILTER = FE_ALL_EXCEPT & ~(FE_UNDERFLOW | FE_INEXACT);
#if WIN32  // windows
    int iExceptionState = std::fetestexcept(FILTER);
#elif __aarch64__  // arm64
    std::uint16_t sw;
    asm volatile (
      "mrs %0, fpsr;"
      : "=r" (sw)
      );
    int iExceptionState = sw & FILTER;
#else  // linux X86_64
    std::uint16_t sw;
    std::uint32_t mxscr;
    asm volatile (
      "fnstsw %0;" // standard FPU status (writes to m16 or ax)
      "stmxcsr %1" // SIMD FPU status (writes to m32)
      : "=a" (sw), "=m" (mxscr)
      );
    int iExceptionState = (sw | mxscr) & FILTER;
#endif // WIN32

    if (iExceptionState == 0) [[likely]]
    {
      // quick exit, when there is no error
      return okay;
    }

    if (iExceptionState & FE_DIVBYZERO)
    {
      std::feclearexcept(FE_ALL_EXCEPT);   //Clear all FPU exceptions
      return firstFPUError;                  // div by zero error
    }

    if (iExceptionState & FE_INVALID)
    {
      std::feclearexcept(FE_ALL_EXCEPT);   //Clear all FPU exceptions
      return static_cast<RETURN_TYPE> (static_cast<std::uint32_t>(firstFPUError) + 1);   // invalid number error
    }

    if (iExceptionState & FE_OVERFLOW)
    {
      std::feclearexcept(FE_ALL_EXCEPT);   //Clear all FPU exceptions
      return static_cast<RETURN_TYPE> (static_cast<std::uint32_t>(firstFPUError) + 2);   // overflow error
    }

    std::feclearexcept(FE_ALL_EXCEPT);   //Clear all FPU exceptions
    return static_cast<RETURN_TYPE> (static_cast<std::uint32_t>(firstFPUError) + 3);     // unknown FPU error
  }

  /// @brief clear all exception status flags in the FPU
  inline void clearFPUFlags()
  {
    std::feclearexcept(FE_ALL_EXCEPT);   //Clear all FPU exceptions
  }

  inline bool g_FPUError {};
}
