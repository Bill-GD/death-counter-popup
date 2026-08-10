#pragma once

#include <Geode/Geode.hpp>
#include <types/types.hpp>

using namespace geode::prelude;

class DeathTrackerHandler {
  const static inline auto PATH = Mod::get()->getSaveDir().parent_path() / "elohmrow.death_tracker" / "levels";
  const static inline auto METADATA_FILENAME = "metadata";
  const static inline auto GENERAL_FILENAME = "general.dt";

public:
  static bool isSaveExists(const std::string& levelID);
  static std::set<std::string> getLinkedLevels(const std::string& levelID);
  static OldDeathCounter getDeaths(const std::string& levelID);
  static OldDeathCounter mergeDeaths(const std::set<std::string>& levelIDs);
  static OldDeathCounter getSaveData(const std::string& levelID);
};
