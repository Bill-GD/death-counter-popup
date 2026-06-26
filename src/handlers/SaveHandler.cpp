#include <handlers/SaveHandler.hpp>

using namespace geode::prelude;

std::string SaveHandler::currentLevelID{};
DeathCounter SaveHandler::deaths{};

bool SaveHandler::isSaveExists(const std::string& levelID) {
  const auto filePath = savePath / (levelID + ".json");
  return std::filesystem::exists(filePath);
}

bool SaveHandler::isDTSaveExists(const std::string& levelID) {
  const auto dirPath = dtPath / levelID;
  return std::filesystem::exists(dirPath) && std::filesystem::exists(dirPath / DT_METADATA_FILENAME);
}

std::set<std::string> SaveHandler::getDTLinkedLevels(const std::string& levelID) {
  const auto filePath = dtPath / levelID / DT_METADATA_FILENAME;

  if (!isDTSaveExists(levelID)) return {};

  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  auto json = readRes.unwrap();
  return json["LinkedLevels"].as<std::set<std::string>>().unwrap();
}

DeathCounter SaveHandler::getDTDeaths(const std::string& levelID) {
  const auto filePath = dtPath / levelID / DT_LEVEL_GENERAL_FILENAME;
  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  auto json = readRes.unwrap();

  auto dtDeaths = json["deaths"].as<DeathCounter>().unwrap();
  const auto dtRuns = json["runs"].as<DeathCounter>().unwrap();

  dtDeaths.insert(dtRuns.begin(), dtRuns.end());

  return dtDeaths;
}

DeathCounter SaveHandler::mergeDTDeaths(const std::set<std::string>& levelIDs) {
  DeathCounter result = {};
  for (const auto& levelID : levelIDs) {
    const auto levelDeaths = getDTDeaths(levelID);
    result.insert(levelDeaths.begin(), levelDeaths.end());
  }
  return result;
}

DeathCounter SaveHandler::getDTSaveData(const std::string& levelID) {
  auto linkedLevels = getDTLinkedLevels(levelID);
  linkedLevels.insert(levelID);
  return mergeDTDeaths(linkedLevels);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID) {
  if (!isSaveExists(levelID)) return {};

  const auto filePath = savePath / (levelID + ".json");
  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  return readRes.unwrap().as<DeathCounter>().unwrap();
}

void SaveHandler::loadSaveData() {
  deaths = getSavedData(currentLevelID);
  if (deaths.empty()) {
    deaths = getDTSaveData(currentLevelID);
  }

  log::info("Loaded saved deaths of level ID={}", currentLevelID);
}
