#include "Revelation.h"
#include "ModuleFactory.h"

std::map<string, ModuleFactory *> *ModuleFactory::m_CommandRegistry;
std::vector<ModuleFactory *> *ModuleFactory::m_ModuleRegistry;

bool ModuleFactory::ExecuteCommand(string szCommand, string szPayload)
{
  if (!m_CommandRegistry)
    return false;

  if (m_CommandRegistry->find(szCommand) == m_CommandRegistry->end())
    return false;

  return (*m_CommandRegistry)[szCommand]->HandleCommand(szPayload);
}

bool ModuleFactory::ExecuteKeypress(DWORD key)
{
  if (!m_ModuleRegistry)
    m_ModuleRegistry = new std::vector<ModuleFactory *>();

  for (std::vector<ModuleFactory *>::iterator i = m_ModuleRegistry->begin(); i != m_ModuleRegistry->end(); ++i) {
    if ((*i)->HandleKeypress(key))
      return true;
  }

  return false;
}

void ModuleFactory::ExecuteUpdate(DWORD time)
{
  if (!m_ModuleRegistry)
    m_ModuleRegistry = new std::vector<ModuleFactory *>();

  for (std::vector<ModuleFactory *>::iterator i = m_ModuleRegistry->begin(); i != m_ModuleRegistry->end(); ++i)
    (*i)->Update(time);
}

void ModuleFactory::ExecuteOnGameStart()
{
  if (!m_ModuleRegistry)
    m_ModuleRegistry = new std::vector<ModuleFactory *>();

  for (std::vector<ModuleFactory *>::iterator i = m_ModuleRegistry->begin(); i != m_ModuleRegistry->end(); ++i)
    (*i)->OnGameStart();
}