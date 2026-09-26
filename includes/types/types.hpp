#pragma once

#include "RunData.hpp"

using RunKey = std::string;
using DeathCounter = std::map<RunKey, RunData>;
using OldDeathCounter = std::map<RunKey, int>;

struct LevelTileInfo {
  std::string id;
  std::string name;
};

struct LevelIDParseResult {
  int id;
  bool isDaily;
  bool isGauntlet;
};

struct KeyedRunData {
  std::string run;
  int count = 0;
  int precision = 0;
  std::string parent;
};
