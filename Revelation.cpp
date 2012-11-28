#include "Revelation.h"
#include "Config.h"
#include "Game.h"

#include <direct.h>

void CRevelation::Log(string text)
{
  static string logFile;
  static bool logInitialized = false;

  if (!logInitialized) {
    logInitialized = true;
    logFile = GetLogPath();
  }

  if (!logFile.empty()) {
    ofstream Log;
    Log.open(logFile.c_str(), ios::app);

    if (!Log.fail()) {
      Log << text << endl;
      Log.close();
    }
  }
}

CRevelation::CRevelation(HMODULE hModule)
{
  m_Game = new CGame(hModule);

  if (!m_Game)
    Log("[Revelation] Failed to create CGame instance!");
}

CRevelation::~CRevelation()
{
  delete m_Game;
  m_Game = NULL;
}

string CRevelation::GetLogPath()
{
  string localPath;
  _getcwd(&localPath[0], 260);

  CConfig cfg;
  string config_path = localPath + string("revelation.cfg");

  cfg.Read(config_path);
  return localPath + cfg.GetString("revelation_log", "revelation.txt");
}
