#include "Revelation.h"
#include "ModuleFactory.h"
#include "Game.h"
#include "JASS.h"

class cmd_strings :  public ModuleFactory
{
public:
  cmd_strings() : ModuleFactory("jstr") { }

private:
  virtual bool HandleCommand(string szPayload) const {
    if (szPayload.empty()) {
      for (unsigned int i = 1; i < getNumStrings(); ++i)
        g_Revelation->m_Game->PrintChat(format("%d: %s") % i % jStrGet((HANDLE)i), 0xFFFFFFFF);
    } else {
      stringstream ss;
      ss << szPayload;

      unsigned int num = 0;
      ss >> num;

      if (num < getNumStrings())
        g_Revelation->m_Game->PrintChat(format("%d: %s") % num % jStrGet((HANDLE)num), 0xFFFFFFFF);
      else
        g_Revelation->m_Game->PrintChat(format("Invalid string number %d.") % num, 0xFFFF0000);
    }

    return true;
  }

  static const cmd_strings m_RegisterThis;
};

#ifdef _DEBUG
const cmd_strings cmd_strings::m_RegisterThis;
#endif