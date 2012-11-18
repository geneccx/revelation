#include "Revelation.h"
#include "Config.h"

CRevelation *g_Revelation;
HMODULE g_hLocalModule;
char g_szLocalPath[MAX_PATH] = { NULL };

extern "C" BOOL APIENTRY DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved)
{
  if (dwReason == DLL_PROCESS_ATTACH) {
    g_hLocalModule = hDll;

    g_Revelation = new CRevelation(g_hLocalModule);
  }

  else if (dwReason == DLL_PROCESS_DETACH) {
    delete g_Revelation;
    g_Revelation = NULL;
  }

  return TRUE;
}