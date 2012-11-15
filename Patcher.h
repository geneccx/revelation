#ifndef PATCHER_H
#define PATCHER_H

class CPatch
{
public:
  CPatch();
  ~CPatch();

  CPatch(DWORD nOffset, BYTE* nData, DWORD nSize);

  bool Apply();
  bool Remove();

  bool Applied() { return m_Applied; }

private:
  DWORD m_Offset;
  BYTE* m_NewData;
  BYTE* m_OldData;
  DWORD m_Size;
  bool m_Applied;
  bool m_Valid;
};

#endif