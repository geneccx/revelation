#ifndef REVELATION_H
#define REVELATION_H

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include <windows.h>
#include <mmsystem.h>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <boost/format.hpp>

using namespace std;
using boost::format;

#include "Encrypt.h"

#define VER "1.0"

class CConfig;
class CGame;

class CRevelation
{
public:
  CRevelation(HMODULE hModule);
  ~CRevelation();

  static string GetLogPath();
  static void Log(string text);
  static void Log(boost::basic_format<char> fmt) { Log(fmt.str()); }

  CGame *m_Game;
};

extern CRevelation *g_Revelation;
#endif