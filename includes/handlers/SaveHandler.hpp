#pragma once

#include "Geode/Geode.hpp"
#include "types/types.hpp"

using namespace geode::prelude;

class SaveHandler {
  static std::string currentLevelName;
  static std::string currentLevelID;

  static std::filesystem::path getLevelPath(const std::string& levelID);
  static bool isSaveExists(const std::string& levelID);

  static DeathCounter getSavedData(const std::string& levelID);
  static DeathCounter getLatestLinkedData();

public:
  const static inline auto PATH = Mod::get()->getSaveDir() / "levels";
  static DeathCounter deaths;

  static bool isLevelSet();
  static void setLevel(GJGameLevel* level);
  static void incrementRun(const std::string& runKey);
  static void loadSaveData();
  static void saveData();
};
