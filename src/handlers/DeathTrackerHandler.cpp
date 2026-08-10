#include "handlers/DeathTrackerHandler.hpp"

#include "utils/FileUtils.hpp"
#include "utils/LevelUtils.hpp"
#include "utils/Utils.hpp"

using namespace geode::prelude;

bool DeathTrackerHandler::isSaveExists(const std::string& levelID) {
  const auto dirPath = PATH / levelID;
  return std::filesystem::exists(dirPath) && std::filesystem::exists(dirPath / METADATA_FILENAME);
}

std::set<std::string> DeathTrackerHandler::getLinkedLevels(const std::string& levelID) {
  if (!LevelUtils::isModLoaded("elohmrow.death_tracker") || !isSaveExists(levelID)) return {};

  const auto filePath = PATH / levelID / METADATA_FILENAME;
  auto [success, val] = FileUtils::tryRead(filePath);

  if (!success) {
    log::info("Failed to read DT metadata from {}", filePath);
    return {};
  }
  // checking for keye explicitly
  // although the value would be null matjson::Value if key doesn't exist, thus can't be parsed -> tryParse returns empty value
  if (!val.contains("LinkedLevels")) return {};
  return Utils::tryParse<std::set<std::string>>(val["LinkedLevels"]);
}

DeathCounter DeathTrackerHandler::getDeaths(const std::string& levelID) {
  const auto filePath = PATH / levelID / GENERAL_FILENAME;
  auto [success, val] = FileUtils::tryRead(filePath);

  if (!success) {
    log::info("Failed to read DT deaths from {}", filePath);
    return {};
  }

  // explicitly check for key
  auto dtDeaths = val.contains("deaths") ? Utils::tryParse<DeathCounter>(val["deaths"]) : DeathCounter{};
  const auto dtRuns = val.contains("runs") ? Utils::tryParse<DeathCounter>(val["runs"]) : DeathCounter{};

  dtDeaths.insert(dtRuns.begin(), dtRuns.end());
  return dtDeaths;
}

DeathCounter DeathTrackerHandler::mergeDeaths(const std::set<std::string>& levelIDs) {
  DeathCounter result = {};
  for (const auto& levelID : levelIDs) {
    for (
      const auto levelDeaths = getDeaths(levelID);
      const auto& [key, value] : levelDeaths
    ) {
      result[key] += value;
    }
  }
  return result;
}

DeathCounter DeathTrackerHandler::getSaveData(const std::string& levelID) {
  auto linkedLevels = getLinkedLevels(levelID);
  linkedLevels.insert(levelID);
  return mergeDeaths(linkedLevels);
}
