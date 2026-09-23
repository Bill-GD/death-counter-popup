#pragma once

#include "Geode/Geode.hpp"
#include "types/LevelInfo.hpp"
#include "types/types.hpp"

using namespace geode::prelude;

class SaveHandler {
  static bool shouldLoad;
  static GJLevelType currentLevelType;
  static std::string currentLevelName;
  static std::string currentLevelID;

  static std::filesystem::path getLevelDataPath(const std::string& levelID);
  static std::filesystem::path getLinkInfoPath(const std::string& levelID);
  static std::filesystem::path getLevelInfoPath(const std::string& levelID);
  static bool isSaveExists(const std::string& levelID);

  static DeathCounter getSavedData(const std::string& levelID);
  static DeathCounter getLatestLinkedData();

  static LevelInfo getLevelInfo();

public:
  const static inline auto PATH = Mod::get()->getSaveDir() / "levels";
  static DeathCounter deaths;

  static bool isLevelSet();
  static bool hasLinkedLevel(const std::string& levelID);
  static std::string getLinkedLevel(const std::string& levelID);
  static void setLevel(GJGameLevel* level);
  static void incrementRun(const std::string& runKey);
  static void loadSaveData();
  static void saveData();

  static LevelInfo getLevelInfo(const std::string& levelID);
};
