#include "Revelation.h"
#include "ModuleFactory.h"
#include "Game.h"
#include "JASS.h"

class mod_cheatpack_detect : public ModuleFactory
{
public:
  mod_cheatpack_detect() : ModuleFactory() { }

private:
  virtual void OnGameStart() const {
    JASSVar *cheatpack = GetJassVar("Activator");
    if (cheatpack) {
      g_Revelation->m_Game->PrintChat("Cheatpack enabled in this game!");
      g_Revelation->m_Game->PrintChat(format("Try: %s") % jStrGet((HANDLE)cheatpack->value));
    }
  }

  static const mod_cheatpack_detect m_RegisterThis;
};

#ifdef _DEBUG
const mod_cheatpack_detect mod_cheatpack_detect::m_RegisterThis;
#endif