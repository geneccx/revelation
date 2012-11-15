#include "Revelation.h"
#include "Config.h"

CConfig::CConfig() {
}

CConfig::~CConfig() {
}

void CConfig::Read(string file) {
  ifstream in;
  in.open(file.c_str());

  if(in.fail())
    return;
  else
  {
    string Line;

    while(!in.eof()) {
      getline(in, Line);

      // ignore blank lines and comments

      if(Line.empty() || Line[0] == '#')
        continue;

      string::size_type Split = Line.find("=");

      if(Split == string::npos)
        continue;

      string::size_type KeyStart = Line.find_first_not_of(" ");
      string::size_type KeyEnd = Line.find(" ", KeyStart);
      string::size_type ValueStart = Line.find_first_not_of(" ", Split + 1);
      string::size_type ValueEnd = Line.size();

      if(ValueStart != string::npos)
        m_CFG[Line.substr(KeyStart, KeyEnd - KeyStart)] = Line.substr(ValueStart, ValueEnd - ValueStart);
    }

    in.close();
  }
}

bool CConfig::Exists(string key) {
  return m_CFG.find(key) != m_CFG.end();
}

int CConfig::GetInt(string key, int x) {
  if(m_CFG.find(key) == m_CFG.end())
    return x;
  else
    return atoi(m_CFG[key].c_str());
}

string CConfig::GetString(string key, string x) {
  if(m_CFG.find(key) == m_CFG.end())
    return x;
  else
    return m_CFG[key];
}