#ifdef WIN32

#include "stdafx.h"

//
// 1. Include Plugin Interface Header
//
#include "TST_PluginInterface.h"

bool APIENTRY DllMain(HMODULE hModule,
  DWORD  ul_reason_for_call,
  LPVOID lpReserved
)
{
  static bool boolMarker = TRUE;
  if (boolMarker)
  {
    //enable memory leak detection
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    boolMarker = FALSE;
  }
#ifdef _DEBUG
  //  _crtBreakAlloc = 406;    // Uncomment to break at a given allocation number.
#endif

  switch (ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  case DLL_THREAD_ATTACH:
  case DLL_THREAD_DETACH:
  case DLL_PROCESS_DETACH:
    break;
  }
  return TRUE;
}

//
// 2. Declare Plugin Meta Information
//
// cppcheck-suppress unknownMacro
PLUGIN_INFO_BEGIN()
PLUGIN_INFO_NAME(L"comm_axisprofile")
PLUGIN_INFO_DEPARTMENT(L"ENG6-CN")
PLUGIN_INFO_RESPONSIBLE(L"PR.ZHANG")
PLUGIN_INFO_DESCRIPTION(L"comm_axisprofile")
PLUGIN_INFO_END()

//
// 3. Declare and implement Plugin TestManager interface
//
PLUGIN_TESTMANAGER_DECLARE()

extern "C"  PLUGIN_API_EXPORT int InitPlugin(iTST_TestApplication *application)
{
  PLUGIN_TESTMANAGER_INIT();
  return 0;
}

extern "C" PLUGIN_API_EXPORT int ReleasePlugin(const iTST_TestApplication *application)
{
  PLUGIN_TESTMANAGER_RELEASE();
  return 0;
}

#endif
