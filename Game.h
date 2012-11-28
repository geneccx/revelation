#ifndef GAME_H
#define GAME_H

struct CGameWar3 {
  DWORD PtrList;           //0x00
  DWORD _1[0x9];           //0x24
  WORD wPlayerSlot;        //0x28
  WORD _2;                 //0x30
  DWORD _3[0x6];           //0x40
  DWORD dwMaxPlayerCount;  //0x44
  DWORD _4;                //0x48
  DWORD dwPlayerCount;     //0x4C
  DWORD _5[0x2];           //0x50
  DWORD dwpPlayerList[15]; //0x58
};

struct UnitInfo {
  DWORD Ptr;             //0x0
  DWORD _1[3];           //0x4
  DWORD XTargetCoord;    //0x8 <- Not sure about these two, but they only appear when a unit is moving
  DWORD YTargetCoord;    //0xC more investigation is needed!
  BYTE _2[0x24];
  DWORD _3[2];           // 0x30
  DWORD MapX;            // 0x34
  DWORD MapY;            // 0x38
};

struct Unit {
  DWORD _1[3];         //0x00
  DWORD ID1;           //0xC
  DWORD ID2;           //0x10
  BYTE _2[0x1C];       //0x2C (0x28 -> Unit Type ..?)
  DWORD ClassId;       //0x30
  BYTE _3[0x24];       //0x54
  DWORD OwnerSlot;     //0x58
  BYTE _4[0x108];      //0x5C
  //1A0
  UnitInfo Info;       //0x164
};

struct Action {
  DWORD vtable;        //+00
  BYTE *PacketData;    //+04
  DWORD _1;            //+08, zero
  DWORD _2;            //+0C, ??
  DWORD Size;          //+10, size of PacketData
  DWORD _3;            //+14, 0xFFFFFFFF
};

struct CNetCommandUnitSelectionModify;

class CDetour;
class CPatch;

class CGame
{
public:
  CGame(HMODULE hModule);
  ~CGame();

  void PrintChat(string str, DWORD aRGB = 0xFF22FF22, float fDuration = 10.0);
  void PrintChat(boost::basic_format<char> fmt, DWORD aRGB = 0xFF22FF22, float fDuration = 10.0) { PrintChat(fmt.str(), aRGB, fDuration); }
  void TextOut(string str, DWORD aRGB = 0xFF22FF22, float fDuration = 7.5);
  void TextOut(boost::basic_format<char> fmt, DWORD aRGB = 0xFF22FF22, float fDuration = 7.5) { TextOut(fmt.str(), aRGB, fDuration); }

  CGameWar3 *GetCGameWar3();
  DWORD GetGameTime() { return m_GameTime ? *(DWORD *)m_GameTime : 0;}
  void *GetObjectFromIDs(DWORD id1, DWORD id2);
  /*  Unit* GetUnitByIDs(DWORD id1, DWORD id2);*/
  void *GetGameState();
  HANDLE GetObjectHandle(void *obj);
  DWORD GetPlayerColor(DWORD slot);

  static DWORD GetGameBase();
  static void UnloadProc();
  DWORD GetGameUI();

private:
  DWORD m_GameBase;
  DWORD m_GetPtrList;
  DWORD m_PrintChat;
  DWORD m_PrintText;
  DWORD m_TextOut;
  DWORD m_GetGameState;
  DWORD m_GetObjectHandle;
  DWORD m_GetPlayerColor;
  DWORD m_GetAgentFromIDs;
  DWORD m_RegisterActionHandler;
  DWORD m_GameInstance;
  DWORD m_GameTime;
  //   DWORD m_GameUI;
  //   DWORD m_WorldFrame;
  DWORD m_HookGameStart;
  DWORD m_HookGameCommand;
  DWORD m_HookDispatchUnitSelectionModify;
  DWORD m_HookDispatchSelectableSelectionModify;
  DWORD m_HookSendAction;

  HMODULE m_hModule;
  static HHOOK m_KeyboardHook;
  static HHOOK m_MessageHook;

  vector<CPatch *> m_Patches;
  void AcquireOffsets();
  void ApplyPatches();
  void RemovePatches();
  void ApplyHooks();
  void RemoveHooks();

  static LRESULT CALLBACK MessageProc(int nCode, WPARAM wParam, LPARAM lParam);
  static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

};

#endif