#include "Revelation.h"
#include "Game.h"
#include "JASS.h"
#include "MHDetect.h"

pair<DWFP, DWFP> GetRandomXYinCircle(float x, float y, float r) {
  pair<DWFP, DWFP> point;

  static float zero = 0.0f;
  static float one = 1.0f;
  static float two_pi = 6.2831853f;

  float angle = jGetRandomReal(&zero, &two_pi).fl;  // random angle between 0 and 2 pi
  r *= jGetRandomReal(&zero, &one).fl;        // scale the radius randomly
  point.first.fl = x + r * jCos(&angle).fl;      // x = x + r*cos(t)
  point.second.fl = y + r * jSin(&angle).fl;      // y = y + r*sin(t)

  return point;
}

void MakeTextTag(const char* msg, float x, float y, float duration = 4.0f) {
  float size = 0.024f;
  float zero = 0.0f;

  RCString* str = MapString(msg);

  HANDLE hTT = jCreateTextTag();
  jSetTextTagText(hTT, str, &size);
  jSetTextTagPos(hTT, &x, &y, &zero);
  jSetTextTagLifespan(hTT, &duration);
  jSetTextTagPermanent(hTT, false);

  delete str;
}

void LocalMessage(const char* msg) {
  float zero = 0.0f;
  float duration = 10.0f;

  RCString* str = MapString(msg); 

  // sadly, GetLocalPlayer doesn't work here.
  for(int i = 0; i < 12; i++)
    jDisplayTimedTextToPlayer(jPlayer(i), &zero, &zero, &duration, str);

  delete str;
}

void __fastcall OnDispatchUnitSelectionModify(CNetCommandUnitSelectionModify* command) {
  if(command->type == 2)
    return;

  unsigned long milliseconds = g_Revelation->m_Game->GetGameTime();
  unsigned long seconds = milliseconds / 1000; milliseconds %= 1000;
  unsigned long minutes = seconds / 60; seconds %= 60;
  unsigned long hours = minutes / 60;  minutes %= 60;

  HANDLE hPlayer = jPlayer(command->playerNum);
  string playerName = jStrGet(jGetPlayerName(hPlayer));
  DWORD playerColor = g_Revelation->m_Game->GetPlayerColor((DWORD)jGetPlayerColor(hPlayer));

  for(unsigned int i = 0; i < command->selectedCount; ++i) {
    void* unit = g_Revelation->m_Game->GetObjectFromIDs(command->selectedUnits[i].ObjectID1, command->selectedUnits[i].ObjectID2);
    if(!unit) {
      CRevelation::Log((format("[%02d:%02d:%02d] Player %s selected invalid unit %X %X")
        % hours  % minutes % seconds
        % playerName
        % command->selectedUnits[i].ObjectID1
        % command->selectedUnits[i].ObjectID2).str());

      continue;
    }

    HANDLE hUnit = g_Revelation->m_Game->GetObjectHandle(unit);
    HANDLE hOwner = jGetOwningPlayer(hUnit);

    static const float d2r = 3.14159f/180.0f;
    float unitX = jGetUnitX(hUnit).fl;
    float unitY = jGetUnitY(hUnit).fl;
    float unitFacing = jGetUnitFacing(hUnit).fl * d2r;
    float ms = jGetUnitMoveSpeed(hUnit).fl/2;        // half ms.. roughly 0.5s delay
    float prevX = unitX - ms * jCos(&unitFacing).fl;
    float prevY = unitY - ms * jSin(&unitFacing).fl;

     if(jGetPlayerAlliance(hOwner, hPlayer, (HANDLE)5)) // can't fogclick ALLIANCE_SHARED_VISION
       continue;
    
    bool locVisible = jIsVisibleToPlayer(&unitX, &unitY, hPlayer);
    bool unitVisible = jIsUnitVisible(hUnit, hPlayer);

     if(locVisible && unitVisible) // can't fogclick visible units
       continue;

    string unitName = jStrGet(jGetUnitName(hUnit));
    string ownerName = jStrGet(jGetPlayerName(hOwner));

    DWORD ownerColor = g_Revelation->m_Game->GetPlayerColor((DWORD)jGetPlayerColor(hOwner));

    string clickType = "fogclicked";

    if(jIsVisibleToPlayer(&prevX, &prevY, hPlayer))
      clickType = "delayclicked?";

    if(locVisible && !unitVisible)
      clickType = "invisclicked";

    CRevelation::Log((format("[%02d:%02d:%02d] Player %s %s %s - %s")
      % hours  % minutes % seconds
      % playerName
      % clickType
      % ownerName
      % unitName).str());

    LocalMessage((format("[%02d:%02d:%02d] |c%08X%s|r %s |c%08X%s|r")
      % hours % minutes % seconds
      % playerColor
      % playerName
      % clickType
      % ownerColor
      % unitName).str().c_str()); 
  }
}

void __fastcall OnDispatchSelectableSelectionModify(CNetCommandSelectableSelectionModify* command) {
  unsigned long milliseconds = g_Revelation->m_Game->GetGameTime();
  unsigned long seconds = milliseconds / 1000; milliseconds %= 1000;
  unsigned long minutes = seconds / 60; seconds %= 60;
  unsigned long hours = minutes / 60;  minutes %= 60;

  HANDLE hPlayer = jPlayer(command->playerNum);
  string playerName = jStrGet(jGetPlayerName(hPlayer));
  DWORD playerColor = g_Revelation->m_Game->GetPlayerColor((DWORD)jGetPlayerColor(hPlayer));

  CNetObjectInfo* object = &command->selectedSelectable;
  void* item = g_Revelation->m_Game->GetObjectFromIDs(object->ObjectID1, object->ObjectID2);
  if(!item) {
    CRevelation::Log((format("[%02d:%02d:%02d] Player %s selected invalid unit %X %X")
      % hours  % minutes % seconds
      % playerName
      % object->ObjectID1
      % object->ObjectID2).str());

    return;
  }

  HANDLE hItem = g_Revelation->m_Game->GetObjectHandle(item);
  float itemX = jGetItemX(hItem).fl;
  float itemY = jGetItemY(hItem).fl;
  
  if(jIsVisibleToPlayer(&itemX, &itemY, hPlayer))
    return;

  string itemName = jStrGet(jGetItemName(hItem));

  CRevelation::Log((format("[%02d:%02d:%02d] Player %s fogclicked item %s")
    % hours  % minutes % seconds
    % playerName
    % itemName).str());

  LocalMessage((format("[%02d:%02d:%02d] |c%08X%s|r fogclicked item %s")
    % hours % minutes % seconds
    % playerColor
    % playerName
    % itemName).str().c_str()); 
}