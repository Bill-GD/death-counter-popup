#include <handlers/DeathTrackerHandler.hpp>
#include <utils/Utils.hpp>

using namespace geode::prelude;

bool DeathTrackerHandler::isSaveExists(const std::string& levelID) {
  const auto dirPath = PATH / levelID;
  return std::filesystem::exists(dirPath) && std::filesystem::exists(dirPath / METADATA_FILENAME);
}

std::set<std::string> DeathTrackerHandler::getLinkedLevels(const std::string& levelID) {
  if (!Utils::isModLoaded("elohmrow.death_tracker") || !isSaveExists(levelID)) return {};

  const auto filePath = PATH / levelID / METADATA_FILENAME;
  auto [success, val] = Utils::tryRead(filePath);

  if (!success) {
    log::info("Failed to read DT metadata from {}", filePath);
    return {};
  }

  return Utils::tryParse<std::set<std::string>>(val["LinkedLevels"]);
}

DeathCounter DeathTrackerHandler::getDeaths(const std::string& levelID) {
  const auto filePath = PATH / levelID / GENERAL_FILENAME;
  auto [success, val] = Utils::tryRead(filePath);

  if (!success) {
    log::info("Failed to read DT deaths from {}", filePath);
    return {};
  }

  auto dtDeaths = Utils::tryParse<DeathCounter>(val["deaths"]);
  const auto dtRuns = Utils::tryParse<DeathCounter>(val["runs"]);

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
