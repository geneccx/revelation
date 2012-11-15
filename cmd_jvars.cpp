#include "Revelation.h"
#include "ModuleFactory.h"
#include "Game.h"
#include "JASS.h"

class cmd_jvars :  public ModuleFactory
{
public:
  cmd_jvars() : ModuleFactory("jvar") { }

private:
  virtual bool HandleCommand(string szPayload) const
  {
    if(!szPayload.empty()) {
      JASSVar* jVar = GetJassVar(szPayload.c_str());

      if(!jVar)
        g_Revelation->m_Game->PrintChat(format("Invalid JASS variable %s.") % szPayload, 0xFFFF0000);
      else
      {
        string type;
        switch(jVar->vartype) {
        case 0:
          type = "void";
          break;
        case 4:
          type = "integer";
          break;
        case 5:
          type = "real";
          g_Revelation->m_Game->PrintChat(format("Found JASS variable %s %s - %.7f") % type % szPayload % *(float*)&jVar->value);
          return true;
        case 6:
          type = "string";
          g_Revelation->m_Game->PrintChat(format("Found JASS variable %s %s - %s") % type % szPayload % jStrGet((HANDLE)jVar->value));
          return true;
        case 8:
          type = "boolean";
          break;
        case 9:
          type = "integer array";
          break;
        case 10:
          type = "real array";
          break;
        case 11:
          type = "string array";
          break;
        case 13:
          type = "boolean array";
          break;
        case 7:
          type = "handle";
          break;
        default:
          type = "unknown";
          break;
        }

        g_Revelation->m_Game->PrintChat(format("Found JASS variable %s %s - %X") % type % szPayload % jVar->value);
      }
    }

    return true;
  }

  static const cmd_jvars m_RegisterThis;
};

#ifdef _DEBUG
const cmd_jvars cmd_jvars::m_RegisterThis;
#endif