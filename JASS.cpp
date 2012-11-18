#include "Revelation.h"
#include "Game.h"

#define _DEFINE_VARS
#include "JASS.h"
#undef _DEFINE_VARS

// JASS Functions

native_info* GetNativeStruct(const char* s) {
  static DWORD g_GetNativeStruct = CGame::GetGameBase() + 0x44EA00;

  typedef native_info* (__fastcall* pGetNativeStruct)(const char* s);
  pGetNativeStruct GetNativeStruct = (pGetNativeStruct)g_GetNativeStruct;
  
  return GetNativeStruct(s);
}

DWORD GetJassVM() {
  static DWORD g_GetJassVM = CGame::GetGameBase() + 0x44B2E0;

  typedef DWORD (__fastcall* pGetJassVM)(int num);
  pGetJassVM GetJassVM = (pGetJassVM)g_GetJassVM;

  return GetJassVM(1);
}

void* GetJASSPtr(const char* s) {
  native_info *ni = GetNativeStruct(s);

  if(!ni) {
    CRevelation::Log(format("[JASS] Failed to get pointer for %s") % s);
    return NULL;
  }

  return (void*)ni->address;
}

RCString* MapString(const char* s) {
  RCString* sr = new RCString;

  typedef RCString* (__thiscall* GAME_MapString_t)(RCString*, const char *);
  static GAME_MapString_t GAME_MapString = (GAME_MapString_t)(CGame::GetGameBase() + 0x011300);

  return GAME_MapString(sr, s);
}

string jStrGet(HANDLE h) {
  DWORD stringMap = *(DWORD*)(GetJassVM() + 0x2874);

  if(!stringMap)
    return "";

  unsigned long strCount = *(DWORD*)(stringMap + 4);

  if((unsigned long)h > strCount)
    return "";

  RCString* strTable = (RCString*)*(DWORD*)(stringMap + 8);

  RCString* str = &((RCString*)strTable)[(unsigned long)h];

  if(!str || !str->data || !str->data->data)
    return "";

  return string(str->data->data);
}

unsigned long getNumStrings() {
  DWORD stringMap = *(DWORD*)(GetJassVM() + 0x2874);

  if(!stringMap)
    return 0;

  return *(DWORD*)(stringMap + 4);
}

JASSVar* GetJassVar(const char* s) {
  DWORD varMap = *(DWORD*)(GetJassVM() + 0x285C);

  if(!varMap)
    return NULL;

  static DWORD g_GetJassVar = CGame::GetGameBase() + 0x44CBC0;

  typedef JASSVar* (__thiscall* pGetJassVar)(DWORD, const char*);
  pGetJassVar GetJassVar = (pGetJassVar)g_GetJassVar;

  return GetJassVar(varMap, s);
}