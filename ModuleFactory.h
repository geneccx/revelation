#pragma once

class ModuleFactory
{
public:
  ModuleFactory() {
    if(!m_ModuleRegistry)
      m_ModuleRegistry = new std::vector<ModuleFactory*>();

    m_ModuleRegistry->push_back(this);
  }

  ModuleFactory(string command, bool registerModule=false) {
    CRevelation::Log((format("[MODULE]Registered command [%1%]") % command).str());

    if(!m_ModuleRegistry)
      m_ModuleRegistry = new std::vector<ModuleFactory*>();

    if(!m_CommandRegistry)
      m_CommandRegistry = new std::map<string, ModuleFactory*>();

    if(registerModule)
      m_ModuleRegistry->push_back(this);

    m_CommandRegistry->insert(std::make_pair(command, this));
  }

  static bool ExecuteCommand(string szCommand, string szPayload);
  static bool ExecuteKeypress(DWORD key);
  static void ExecuteUpdate(DWORD time);
  static void ExecuteOnGameStart();

protected:
  static std::map<string, ModuleFactory*>* m_CommandRegistry;
  static std::vector<ModuleFactory*>* m_ModuleRegistry;

  virtual bool HandleCommand(string szPayload) const { return false; }
  virtual bool HandleKeypress(DWORD key) const { return false; }
  virtual void Update(DWORD time) const { }
  virtual void OnGameStart() const { }
};
