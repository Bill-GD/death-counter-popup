#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class LevelUtils {
public:
  static const char* levelTypeToString(GJLevelType type);
  static std::string formatPercent(const float& percent, const float& maxClamp = 100.f);
  static std::pair<std::string, std::string> computeRunKeys(const std::string& key);
  static std::string getParentKey(const std::string& key);
  static std::vector<std::string> getAllParentKeys(const std::string& key);
  /**
   * @param key The max precision key, provided by [formatPercent]
   * @param precision The target precision, clamped to [0, MAX_PRECISION]
   */
  static std::string getKeyByPrecision(const std::string& key, const int& precision);
  static int getKeyPrecision(const std::string& key);

  static bool isLevelCompleted(GJGameLevel* level);
  static bool isGauntletLevel(const GJGameLevel* level);
  static bool isDailyLevel(GJGameLevel* level);

  static std::string getLevelID(GJGameLevel* level);
  static bool isModLoaded(const std::string& modID);
  // static EndTriggerGameObject* getLastEndTrigger(const PlayLayer* layer);
  // static EndPortalObject* getEndWall(const PlayLayer* layer);

  static void dumpLevelInfo(GJGameLevel* level);
  // static void dumpAttemptInfo();
};
