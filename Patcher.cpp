#include "Revelation.h"
#include "Patcher.h"

CPatch::CPatch() {
  m_Offset = NULL;
  m_NewData = NULL;
  m_OldData = NULL;
  m_Size = 0;

  m_Applied = false;
  m_Valid = false;
}

CPatch::~CPatch() {
  if(m_Applied)
    Remove();

  m_Offset = NULL;
  m_Size = 0;

  delete[] m_NewData;
  delete[] m_OldData;
}

CPatch::CPatch(DWORD nOffset, BYTE* nData, DWORD nSize) {
  m_Applied = false;
  m_Valid = false;

  DWORD dwOldProt, dwDummy;

  m_Offset = nOffset;
  m_Size = nSize;

  m_NewData = new BYTE[ m_Size ];
  m_OldData = new BYTE[ m_Size ];

  if(!memcpy(m_NewData, nData, m_Size)) return;
  if(!VirtualProtect((LPVOID)m_Offset, m_Size, PAGE_EXECUTE_READWRITE, &dwOldProt)) return;
  if(!memcpy(m_OldData, (LPVOID)m_Offset, m_Size)) return;
  VirtualProtect((LPVOID)m_Offset, m_Size, dwOldProt, &dwDummy);

  m_Valid = true;
  Apply();
}

bool CPatch::Apply() {
  if(!m_Valid || m_Applied)
    return false;

  DWORD dwOldProt, dwDummy;

  if(!VirtualProtect((LPVOID)m_Offset, m_Size, PAGE_EXECUTE_READWRITE, &dwOldProt))
    return false;

  if(!memcpy((LPVOID)m_Offset, m_NewData, m_Size))
    return false;

  VirtualProtect((LPVOID)m_Offset, m_Size, dwOldProt, &dwDummy);

  m_Applied = true;

  return true;
}

bool CPatch::Remove() {
  if(!m_Valid || !m_Applied)
    return false;

  DWORD dwOldProt, dwDummy;

  if(!VirtualProtect((LPVOID)m_Offset, m_Size, PAGE_EXECUTE_READWRITE, &dwOldProt))
    return false;

  if(!memcpy((LPVOID)m_Offset, m_OldData, m_Size))
    return false;

  VirtualProtect((LPVOID)m_Offset, m_Size, dwOldProt, &dwDummy);

  m_Applied = false;

  return true;
}