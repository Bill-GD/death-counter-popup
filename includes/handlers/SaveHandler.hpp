#pragma once

#include <Geode/Geode.hpp>
#include <types/types.hpp>

using namespace geode::prelude;

class SaveHandler {
  const static inline auto DT_METADATA_FILENAME = "metadata";
  const static inline auto DT_LEVEL_GENERAL_FILENAME = "general.dt";

  const static inline auto dtPath = Mod::get()->getSaveDir().parent_path() / "elohmrow.death_tracker" / "levels";
  const static inline auto savePath = Mod::get()->getSaveDir() / "levels"; // .json

  static bool isSaveExists(const std::string& levelID);
  static bool isDTSaveExists(const std::string& levelID);
  static std::set<std::string> getDTLinkedLevels(const std::string& levelID);
  static std::map<std::string, int> getDTDeaths(const std::string& levelID);
  static DeathCounter mergeDTDeaths(const std::set<std::string>& levelIDs);
  static DeathCounter getDTSaveData(const std::string& levelID);
  static DeathCounter getSavedData(const std::string& levelID);

public:
  static DeathCounter deaths;
  static std::string currentLevelID;

  static void loadSaveData();
  // static void saveData(std::string levelID);
};
