#pragma once

#include "RunData.hpp"

using RunKey =  std::string;
using DeathCounter =  std::map<RunKey, RunData>;
using OldDeathCounter =  std::map<RunKey, int>;

struct LevelTileInfo {
  std::string id;
  std::string name;
};
