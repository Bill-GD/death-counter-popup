#pragma once

#include "RunData.hpp"

using RunKey = std::string;
using DeathCounter = std::map<RunKey, RunData>;
using OldDeathCounter = std::map<RunKey, int>;

struct LevelTileInfo {
  std::string id;
  std::string name;
};

struct LevelInfoFromFileResult {
  std::string id;
  std::string name;
  std::string link;
};

struct LevelIDParseResult {
  int id;
  GJLevelType type;
  bool isDaily;
  bool isGauntlet;
};

struct FetchSavedLevelResult {
  int id;
  GJLevelType type;
  GJGameLevel* level;
};

struct KeyedRunData {
  std::string run;
  int count = 0;
  int precision = 0;
  std::string parent;
};
