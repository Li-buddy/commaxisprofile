#pragma once

#ifdef WIN32

// Modify the following defines if you have to target a platform prior to the ones specified below.
// Refer to MSDN for the latest info on corresponding values for different platforms.
#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#define NOMINMAX
#include <windows.h>
#include <algorithm>
#else

typedef long HRESULT;
#define S_OK                                   ((HRESULT)0L)
#define S_FALSE                                ((HRESULT)1L)
#define E_FAIL                                 ((HRESULT)0x80000008L)

// macros from winerror.h 
#define FAILED(hr) (((HRESULT)(hr)) < 0)
#define SUCCEEDED(hr) (((HRESULT)(hr)) >= 0)


#define ARRAYSIZE(a) \
    ((sizeof(a) / sizeof(*(a))) / \
    static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

#include <string.h>
#include <cfloat>
#endif

#include "globtype.h"
#include <stdarg.h>
#include "xTST_Test.h"
#include "motion/test/string_helper.h"

#define EMAIL_MA  "Michael.Albrecht@boschrexroth.de"
#define EMAIL_JD  "Joerg.Deisenroth@boschrexroth.de"
#define EMAIL_ME  "Maximilian.Elsesser@boschrexroth.de"
#define EMAIL_JF  "Jascha.Friedrich@boschrexroth.de"
#define EMAIL_PEG "Peter.Groebner@boschrexroth.de"
#define EMAIL_RAK "Ralph.Kaulvers@boschrexroth.de"
#define EMAIL_RL  "Robin.Leblebici@boschrexroth.de"
#define EMAIL_TS  "Thomas.Schroeder4@boschrexroth.de"
#define EMAIL_DS  "Dieter.Streit@boschrexroth.de"
#define EMAIL_MW  "Markus.Wehner@boschrexroth.de"
#define EMAIL_JK  "Julian.Kaufmann@boschrexroth.de"
#define EMAIL_UJ  "Uwe.Junkes@boschrexroth.de"
#define EMAIL_SD  "Sikai.Dong@boschrexroth.com.cn"
#define EMAIL_LLH "Linhan.LI@boschrexroth.com.cn"
#define EMAIL_PRZ "Peiran.ZHANG@boschrexroth.com.cn"
#define EMAIL_JG  "Jing.GUO@boschrexroth.com.cn"
#define EMAIL_JS  "Jin.Sha@boschrexroth.com.cn"
#define EMAIL_PFL "Pengfei.Liu@boschrexroth.com.cn"
#define EMAIL_XQF "Xiaoqiang.Feng@boschrexroth.com.cn"
#define EMAIL_ML  "Meng.Liu@boschrexroth.com.cn"
#define EMAIL_YSK "Shangkun.YANG@boschrexroth.com.cn"
#define EMAIL_LLJ "Linjie.LI@boschrexroth.com.cn"
#define EMAIL_LQ  "Qun.Liu@boschrexroth.com.cn"
#define EMAIL_WSY "was3xi@boschrexroth.com.cn"
#define EMAIL_LST "Shutong.LIU@boschrexroth.com.cn"

#define APICALL_AND_CHECK(function) \
  result = function; \
  if (FAILED(result)) \
  { \
  error(L"failed to call " L#function L" with return 0x%08x, line %d in file %s", result, __LINE__, __FILEW__); \
  };

#define APICALL_AND_CHECK_ERR_RETURN(function) \
  result = function; \
  if (FAILED(result)) \
  { \
  error(L"failed to call " L#function L" with return 0x%08x, line %d in file %s", result, __LINE__ ,__FILEW__); \
  return result; \
  };


#include "malloc.h"

//! @brief Macro to convert a const char* to wchar*
//! @param[in]   strIn   Input string as const char*
//! @return      Converted string as wchar*
#define UTF8TOWIDE(strIn) (test::StringHelper::utf8ToWide(strIn).c_str())

inline char* _wcstombs(char *dst, const wchar_t *src, size_t len)
{
  // same as wcstombs16, but returns pointer to dst instead of length
  wcstombs(dst, src, len);
  return dst;
}

inline wchar_t* _mbstowcs(wchar_t *dst, const char *src, size_t len)
{
  // same as mbstowcs16, but returns pointer to dst instead of length
  mbstowcs(dst, src, len);
  return dst;
}
