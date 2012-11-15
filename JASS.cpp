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

unsigned long __declspec() callfunc(void* addr, int nargs, int* args) {
  __asm
  {
    push ebp;
    mov ebp, esp;
    sub esp, 8;
  }

  for(int i = nargs - 1; i >= 0; i--) {
    int arg;
    arg = args[i];

    __asm push arg
  }

  __asm
  {
    call addr;
    leave;
    ret;
  }
}

int CallJassNative(char* name, ...) {
  native_info *ni = GetNativeStruct(name);

  if(!ni)
    return -1;

  int nargs = 0;
  int inhandle = false;
  int arglist[32] = {0};

  va_list vl;
  va_start(vl, name);

  for(int i=0;ni->prototype[i]!=')';i++) {
    if(inhandle && ni->prototype[i]==';')
      inhandle = false;
    else
    {
      if(ni->prototype[i] == 'H') {
        inhandle = true;
        arglist[nargs] = va_arg(vl, DWORD);
        nargs++;
      }
      else if(ni->prototype[i] == 'I') {
        arglist[nargs] = va_arg(vl, DWORD);
        nargs++;
      }
      else if(ni->prototype[i] == 'S') {
        arglist[nargs] = (int)va_arg(vl, RCString*);
        nargs++;
      }
      else if(ni->prototype[i] == 'B') {
        arglist[nargs] = va_arg(vl, bool);
        nargs++;
      }
      else if(ni->prototype[i] == 'C') {
        arglist[nargs] = va_arg(vl, DWORD);
        nargs++;
      }
      else if(ni->prototype[i] == 'R') {
        float f = (float)va_arg(vl, double);
        arglist[nargs] = *(DWORD*)&f;
        nargs++;
      }
    }
  }

  va_end(vl);

  unsigned long ret = callfunc((void *)ni->address, nargs, arglist);
  return ret;
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