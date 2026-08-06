#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class LevelUtils {
public:
  static const char* levelTypeToString(GJLevelType type);
  static std::string formatPercent(const float& percent);

  static bool isLevelCompleted(GJGameLevel* level);
  static bool isGauntletLevel(const GJGameLevel* level);
  static bool isDailyLevel(GJGameLevel* level);

  static std::string getLevelID(GJGameLevel* level);

  static bool isModLoaded(const std::string& modID);

  static void dumpLevelInfo(GJGameLevel* level);
};
