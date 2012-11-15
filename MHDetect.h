#ifndef MHDetect_h__
#define MHDetect_h__

struct CNetObjectInfo
{
  DWORD ObjectID1;
  DWORD ObjectID2;
};

struct CNetCommandUnitSelectionModify
{
  BYTE _1[0x15];
  BYTE playerNum;
  WORD _2;
  BYTE type;                        // 1 = select, 2 = deselect
  BYTE _3[0x7];
  DWORD selectedCount;
  CNetObjectInfo* selectedUnits;
};

struct CNetCommandSelectableSelectionModify
{
  BYTE _1[0x15];
  BYTE playerNum;                    // 0x15
  WORD _2;                           // 0x16
  BYTE type;                         // always 4?  0x18
  BYTE _3[0x3];                      // 0x19
  CNetObjectInfo selectedSelectable; //0x1C
};

void __fastcall OnDispatchUnitSelectionModify(CNetCommandUnitSelectionModify* command);
void __fastcall OnDispatchSelectableSelectionModify(CNetCommandSelectableSelectionModify* command);
#endif // MHDetect_h__