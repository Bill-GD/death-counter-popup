#pragma once

#include <Geode/Geode.hpp>
#include <types/types.hpp>

using namespace geode::prelude;

class DeathTrackerHandler {
  const static inline auto PATH = Mod::get()->getSaveDir().parent_path() / "elohmrow.death_tracker" / "levels";
  const static inline auto METADATA_FILENAME = "metadata";
  const static inline auto GENERAL_FILENAME = "general.dt";

  static std::pair<bool, matjson::Value> tryRead(const std::filesystem::path& filePath);

  template <typename T>
  static T tryParse(const matjson::Value& value);

public:
  static bool isSaveExists(const std::string& levelID);
  static std::set<std::string> getLinkedLevels(const std::string& levelID);
  static std::map<std::string, int> getDeaths(const std::string& levelID);
  static DeathCounter mergeDeaths(const std::set<std::string>& levelIDs);
  static DeathCounter getSaveData(const std::string& levelID);
};
