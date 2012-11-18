#include "Revelation.h"
#include "Game.h"
#include "Detour/CDetour.h"
#include "ModuleFactory.h"
#include "Patcher.h"
#include "MHDetect.h"

#define PATCH(a1, a2, a3)  m_Patches.push_back(new CPatch((m_GameBase + a1), (BYTE*)a2, a3))

CGame::CGame(HMODULE hModule) : m_hModule(hModule)
{
  AcquireOffsets();
  ApplyPatches();
  ApplyHooks();
}

CGame::~CGame()
{
  RemovePatches();
  RemoveHooks();

  m_GameBase           = NULL;
  m_GetPtrList         = NULL;
  m_PrintChat          = NULL;
  m_TextOut            = NULL;
  m_GetGameState       = NULL;
  m_GetObjectHandle    = NULL;
  m_GetPlayerColor     = NULL;
  m_GetAgentFromIDs    = NULL;

  m_GameInstance       = NULL;
  m_GameTime           = NULL;
  // m_GameUI           = NULL;
  // m_WorldFrame       = NULL;
}

void CGame::AcquireOffsets()
{
  m_GameBase = GetGameBase();

  if (!m_GameBase) {
    CRevelation::Log("[GAME] Could not get game base!");
    return;
  }

  m_GetPtrList                            = m_GameBase + 0x5FAEC0;
  m_PrintChat                             = m_GameBase + 0x6049B0;
  m_TextOut                               = m_GameBase + 0x2F3CB0;
  m_GetGameState                          = m_GameBase + 0x3A8060;
  m_GetObjectHandle                       = m_GameBase + 0x430C80;
  m_GetPlayerColor                        = m_GameBase + 0x314B60;
  m_GetAgentFromIDs                       = m_GameBase + 0x03FA30;

  m_GameInstance                          = m_GameBase + 0xAB65F4;
  m_GameTime                              = m_GameBase + 0xAB7E98;
  //m_GameUI                              = m_GameBase + 0xACE66C;
  //m_WorldFrame                          = m_GameBase + 0xACE670;

  // Hooks

  m_HookGameStart                         = m_GameBase + 0x5C0D69;
  m_HookGameCommand                       = m_GameBase + 0x2F93B0;
  m_HookDispatchUnitSelectionModify       = m_GameBase + 0x2CEC70;
  m_HookDispatchSelectableSelectionModify = m_GameBase + 0x2CCC10;
  m_HookSendAction                        = m_GameBase + 0x54D970;

}

//
// Game API
//

DWORD CGame::GetGameBase()
{
  string DllName = "Game.dll";
  DWORD dwGameBase = (DWORD)GetModuleHandle(DllName.c_str());

  return dwGameBase ? dwGameBase : (DWORD)LoadLibrary(DllName.c_str());
}

DWORD CGame::GetGameUI()
{
  if (!m_GetPtrList)
    return NULL;

  typedef DWORD GAME_GetPtrList_t();
  static GAME_GetPtrList_t *GAME_GetPtrList = (GAME_GetPtrList_t *)m_GetPtrList;

  return GAME_GetPtrList();
}

void CGame::PrintChat(string str, DWORD aRGB, float fDuration)
{
  if (!m_PrintChat || !GetGameUI())
    return;

  class CSimpleMessageFrame { };

  typedef void (__thiscall * GAME_PrintChat_t)(CSimpleMessageFrame *, const char *, DWORD *, float, int);
  static GAME_PrintChat_t GAME_PrintChat = reinterpret_cast<GAME_PrintChat_t>(m_PrintChat);

  //   messageFrame = *(CSimpleMessageFrame**)(GetGameUI() + 0x3EC);
  //   using namespace std::placeholders;
  //   auto f = bind(GAME_PrintChat, messageFrame, _1, _2, _3, _4);
  //   f(str.c_str(), &aRGB, fDuration, 0);
  GAME_PrintChat(*(CSimpleMessageFrame **)(GetGameUI() + 0x3EC), str.c_str(), &aRGB, fDuration, 0);
}

void CGame::TextOut(string str, DWORD aRGB, float fDuration)
{
  if (!m_TextOut)
    return;

  typedef void (__thiscall * GAME_TextOut_t)(DWORD, const char *, float, DWORD);
  static GAME_TextOut_t GAME_TextOut = (GAME_TextOut_t)m_TextOut;

  GAME_TextOut(GetGameUI(), str.c_str(), fDuration, aRGB);
}

CGameWar3 *CGame::GetCGameWar3()
{
  if (!m_GameInstance)
    return NULL;

  return *(CGameWar3 **)m_GameInstance;
}

void *CGame::GetObjectFromIDs(DWORD id1, DWORD id2)
{
  if (!m_GetAgentFromIDs)
    return NULL;

  typedef DWORD (__fastcall * GAME_GetAgentFromIDs_t)(DWORD, DWORD);
  static GAME_GetAgentFromIDs_t GAME_GetAgentFromIDs = (GAME_GetAgentFromIDs_t)m_GetAgentFromIDs;

  DWORD result = GAME_GetAgentFromIDs(id1, id2);

  if (result && *(DWORD *)(result + 0xC) == 0x2B61676C)
    return (void *) * (DWORD *)(result + 0x54);

  return NULL;
}

void *CGame::GetGameState()
{
  if (!m_GetGameState)
    return NULL;

  typedef void* (__thiscall * GAME_GetGameState_t)(CGameWar3 *);
  static GAME_GetGameState_t GAME_GetGameState = (GAME_GetGameState_t)m_GetGameState;

  return GAME_GetGameState(GetCGameWar3());
}

HANDLE CGame::GetObjectHandle(void *obj)
{
  if (!m_GetObjectHandle)
    return NULL;

  typedef HANDLE (__thiscall * GAME_GetObjectHandle_t)(void *, void *, int);
  static GAME_GetObjectHandle_t GAME_GetObjectHandle = (GAME_GetObjectHandle_t)m_GetObjectHandle;

  return GAME_GetObjectHandle(GetGameState(), obj, 1);
}

DWORD CGame::GetPlayerColor(DWORD slot)
{
  if (!m_GetPlayerColor)
    return 0xFFFFFFFF;

  typedef DWORD* (__fastcall * GAME_GetPlayerColor_t)(DWORD);
  static GAME_GetPlayerColor_t GAME_GetPlayerColor = (GAME_GetPlayerColor_t)m_GetPlayerColor;

  return (slot > 12) ? 0xFFFFFFFF : *GAME_GetPlayerColor(slot);
}

//
// Patch functions
//

void CGame::ApplyPatches()
{
  //   PATCH(0x431556,  "\x3B\xC0\x0F\x85\xC0\x00\x00\x00\x8D\x8B"
  //       "\xF0\x00\x00\x00\xE8\x07\x3D\x03\x00\x3B"
  //       "\xC0\x0F\x85\xAD\x00\x00\x00",27);    // Show pings
  //   PATCH(0x3A1E9B,  "\x90\x90",2);        // Show units in game
  //   PATCH(0x361DFC,  "\x00",1);          // Show units on minimap
  //   PATCH(0x285B8C,  "\x90\x90",2);        // Clickable units
  //   PATCH(0x285BA2,  "\xEB\x29",2);        // Clickable units 2
  //   PATCH(0x28345C,  "\x40\xc3",2);        // Reveal Illusions
  //   PATCH(0x73DEC9,  "\xB2\x00\x90\x90\x90\x90",6);    // Remove fog
  //   PATCH(0x362211,  "\x3B\xC0\x0F\x85\x30\x04\x00\x00",8);  // Show invisible units on minimap
  //   PATCH(0x356E7E,  "\x90\x90\x90",3);        // Show invisible units on minimap 2
  //   PATCH(0x3C5C22,  "\xEB",1);          // Bypass -ah
  //   PATCH(0x3C135C,  "\xB8\xFF\x00\x00\x00\xEB",6);    // Bypass -ah 2
  //   PATCH(0x3F92CA,  "\x90\x90",2);        // Viewable items
  //   PATCH(0x3A1DDB,  "\xEB",1);          // Viewable items 2
  //   PATCH(0x2030DC,  "\x90\x90\x90\x90\x90\x90",6);    // Viewable skills
  //   PATCH(0x34FC68,  "\x90\x90",2);        // Viewable skills 2
  //   PATCH(0x28EBCE,  "\xEB",1);          // View cooldowns
  //   PATCH(0x34FCA6,  "\x90\x90\x90\x90",4);      // View cooldowns 2
  //   PATCH(0x285BA2,  "\xEB",1);          // Click invisible units
  //   PATCH(0x34E762,  "\xB8\x00\x00\x00\x00\x90",6);    // High trade amount
  //   PATCH(0x34E763,  "\xC8\x00\x00\x00",4);      // = 200
  //   PATCH(0x34E76A,  "\xB8\x00\x00\x00\x00\x90",6);    // Low trade amount
  //   PATCH(0x34E76B,  "\x64\x00\x00\x00",4);      // = 100

  //   NPATCH(0x4559EC,"\x60\x0B\xC0\x75\x2F\x8A\x8E\xD0\x02\x00\x00\x80\xF9\x01\x74\x51\xC6\x86\xD0\x02\x00\x00\x01\xC6\x86\xD1\x02\x00\x00\x01\xC6\x86\xD2\x02\x00\x00\xFF\x8B\xCE\xB8\xFF\x01\x01\xFF\xFF\x15\x60\x5A\x45\x00\xEB\x2D\x8A\x8E\xD0\x02\x00\x00\x80\xF9\xFF\x74\x22\xC6\x86\xD0\x02\x00\x00\xFF\xC6\x86\xD1\x02\x00\x00\xFF\xC6\x86\xD2\x02\x00\x00\xFF\x8B\xCE\xB8\xFF\xFF\xFF\xFF\xFF\x15\x60\x5A\x45\x00\x61\xB8\x01\x00\x00\x00\x23\xD8\x89\x44\x24\xE4\xFF\x35\x65\x5A\x45\x00\xC3",116);
  //   DWORD addr[3];
  //   addr[0] = m_GameBase + 0x29E270; //References to the Game.dll
  //   addr[1] = m_GameBase + 0x39A3BF; //
  //   addr[2] = 0x4559EC;    //Detour
  //   NPATCH(0x455A60,&addr[0],sizeof(DWORD));
  //   NPATCH(0x455A65,&addr[1],sizeof(DWORD));
  //   NPATCH(0x455A6A,&addr[2],sizeof(DWORD));
  //  PATCH(0x39A3B9,"\xFF\x25\x6A\x5A\x45\x00",6); //Plant detour
}

void CGame::RemovePatches()
{
  for (vector<CPatch *>::iterator i = m_Patches.begin(); i != m_Patches.end(); i++) {
    if ((*i)->Applied())
      (*i)->Remove();

    delete (*i);
  }
}

//
// Hooking functions
//

CDetour GameStartDet;
CDetour GameChatDet;
CDetour DispatchUnitSelectionModifyDet;
CDetour DispatchSelectableSelectionModifyDet;

void OnGameStart();
bool __fastcall OnGameCommand(DWORD, DWORD, char *);

void CGame::ApplyHooks()
{
  GameStartDet.Detour((BYTE *)m_HookGameStart, (BYTE *)OnGameStart, true);
  GameStartDet.Apply();

#ifdef _DEBUG
  GameChatDet.Detour((BYTE *)m_HookGameCommand, (BYTE *)OnGameCommand, true);
  GameChatDet.Apply();
#endif

  DispatchUnitSelectionModifyDet.Detour((BYTE *)m_HookDispatchUnitSelectionModify, (BYTE *)OnDispatchUnitSelectionModify, true);
  DispatchUnitSelectionModifyDet.Apply();

  DispatchSelectableSelectionModifyDet.Detour((BYTE *)m_HookDispatchSelectableSelectionModify, (BYTE *)OnDispatchSelectableSelectionModify, true);
  DispatchSelectableSelectionModifyDet.Apply();

#ifdef _DEBUG
  m_KeyboardHook = SetWindowsHookEx(WH_KEYBOARD, KeyboardProc, m_hModule, GetCurrentThreadId());
  m_MessageHook  = SetWindowsHookEx(WH_GETMESSAGE, MessageProc, m_hModule, GetCurrentThreadId());
#endif
}

void CGame::RemoveHooks()
{
  if (GameStartDet.Applied())
    GameStartDet.Remove();

  if (GameChatDet.Applied())
    GameChatDet.Remove();

  if (DispatchUnitSelectionModifyDet.Applied())
    DispatchUnitSelectionModifyDet.Remove();

  if (DispatchSelectableSelectionModifyDet.Applied())
    DispatchSelectableSelectionModifyDet.Remove();

  if (m_KeyboardHook)
    UnhookWindowsHookEx(m_KeyboardHook);

  if (m_MessageHook)
    UnhookWindowsHookEx(m_MessageHook);
}

DWORD g_dwUnloadRetAddr = 0;

void CGame::UnloadProc()
{
  asm(
    "push %0;"
    "push %1;"
    "jmpl *%2;"
    :: "r" (GetModuleHandle(0)), "m" (g_dwUnloadRetAddr), "m" (FreeLibrary)
  );
}

//
// Hook Callbacks
//

bool __fastcall OnGameCommand(DWORD _ecx, DWORD _edx, char *nText)
{
  bool bRet = true;
  string Message = string(nText);

  if (!Message.empty() && Message[0] == '/') {
    string szCommand;
    string szPayload;
    string::size_type PayloadStart = Message.find(" ");

    if (PayloadStart != string::npos) {
      szCommand = Message.substr(1, PayloadStart - 1);
      szPayload = Message.substr(PayloadStart + 1);
    } else
      szCommand = Message.substr(1);

    transform(szCommand.begin(), szCommand.end(), szCommand.begin(), (int( *)(int))tolower);

    if (szCommand == "unload") {
      g_dwUnloadRetAddr = (DWORD)GameChatDet.GetGateRetAddress();
      GameChatDet.SetGateRetAddress((BYTE *)&CGame::UnloadProc);
      g_Revelation->m_Game->TextOut("Revelation unloaded.", 0xFFFF0000);
      GameChatDet.Ret(false);
      return true;
    } else {
      bRet = !ModuleFactory::ExecuteCommand(szCommand, szPayload);
    }
  }

  GameChatDet.Ret(bRet);
  return !bRet;
}

void OnGameStart()
{
#include "JASS.h"
  g_Revelation->m_Game->TextOut(/*"Revelation v1.0 by Zephyrix is active."); */
    XorStr<0x4F, 39, 0x675FA9CA>("\x1D\x35\x27\x37\x3F\x35\x21\x3F\x38\x36\x79\x2C\x6A\x72\x6D\x7E\x3D\x19\x41\x38\x06\x14\x0D\x1F\x15\x01\x11\x4A\x02\x1F\x4D\x0F\x0C\x04\x18\x04\x16\x5A" + 0x675FA9CA).s);
  CRevelation::Log("[GAME] Started a new game.");

  ModuleFactory::ExecuteOnGameStart();
}

LRESULT CALLBACK CGame::KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
#define IS_KEY_DOWN(lParam) ((lParam & ((DWORD)1<<30)) == 0 && (lParam & ((DWORD)1<<31)) == 0)

  if (nCode == HC_ACTION && IS_KEY_DOWN(lParam)) {
    if (ModuleFactory::ExecuteKeypress(lParam))
      return TRUE;
  }

  return CallNextHookEx(m_KeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK CGame::MessageProc(int nCode, WPARAM wParam, LPARAM lParam)
{
  ModuleFactory::ExecuteUpdate(GetTickCount());

  return CallNextHookEx(m_MessageHook, nCode, wParam, lParam);
}

HHOOK CGame::m_KeyboardHook;
HHOOK CGame::m_MessageHook;