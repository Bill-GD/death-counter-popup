#pragma once

#include <Geode/Geode.hpp>
#include <types/types.hpp>

using namespace geode::prelude;

class SaveHandler {
  const static inline auto DT_METADATA_FILENAME = "metadata";
  const static inline auto DT_LEVEL_GENERAL_FILENAME = "general.dt";

  const static inline auto dtPath = Mod::get()->getSaveDir().parent_path() / "elohmrow.death_tracker" / "levels";

  static std::string currentLevelName;
  static std::string currentLevelID;

  static bool isDTSaveExists();
  static std::set<std::string> getDTLinkedLevels();
  static std::map<std::string, int> getDTDeaths(const std::string& levelID);
  static DeathCounter mergeDTDeaths(const std::set<std::string>& levelIDs);
  static DeathCounter getDTSaveData();

  static bool isSaveExists(const std::string& levelID);
  static DeathCounter getSavedData(const std::string& levelID);
  static DeathCounter getLatestLinkedData();

  static bool tryWrite(const std::filesystem::path& filePath, const matjson::Value& value);

public:
  const static inline auto savePath = Mod::get()->getSaveDir() / "levels"; // .json

  static DeathCounter deaths;

  static void setLevel(const std::string& id, const std::string& name);
  static void loadSaveData();
  static void updateDeath(const std::string& death);
  static void saveData();
};
