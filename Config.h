#ifndef CONFIG_H
#define CONFIG_H

class CConfig
{
private:
  map<string, string> m_CFG;
public:
  CConfig();
  ~CConfig();

  void Read(string file);
  bool Exists(string key);
  int GetInt(string key, int x);
  string GetString(string key, string x);
};

#endif