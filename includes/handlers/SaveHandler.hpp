#pragma once

#include "Geode/Geode.hpp"
#include "types/LevelInfo.hpp"
#include "types/types.hpp"

using namespace geode::prelude;

enum class SavePathType {
  DIR,
  DATA,
  INFO,
  LINK,
};

class SaveHandler {
  static bool shouldLoad;
  static GJLevelType currentLevelType;
  static std::string currentLevelName;
  static std::string currentLevelID;

  static std::filesystem::path getLevelPath(const std::string& levelID, SavePathType type);
  static bool pathExists(const std::string& levelID, SavePathType type);

  static DeathCounter getLatestLinkedData();

  static LevelInfo getLevelInfo();
  static void saveCurrentLevelInfo();

public:
  const static inline auto PATH = Mod::get()->getSaveDir() / "levels";
  static DeathCounter deaths;

  static const std::string& getCurrentLevelID();

  static bool isLevelSet();
  static bool hasLinkedLevel(const std::string& levelID);
  static std::string getLinkedLevel(const std::string& levelID);
  static void setLevel(GJGameLevel* level);
  static void incrementRun(const std::string& runKey);
  static DeathCounter getSavedData(const std::string& levelID);
  static void loadSaveData();
  static void saveData();
  static bool deleteSavedData(const std::string& levelID);

  static LevelInfoFromFileResult getLevelInfoFromFile(const std::string& levelID, bool shouldLog = true);
  static LevelSaveDataMetadata getMetadata(const std::string& levelID);
};
