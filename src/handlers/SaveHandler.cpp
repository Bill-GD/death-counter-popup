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

  saveData();
  log::info("Loaded saved deaths of level ID={}", currentLevelID);
  // log::info("Loaded data: {}", deaths);
}

void SaveHandler::updateDeath(const std::string& death) {
  deaths[death]++;
}

bool SaveHandler::tryWrite(const std::filesystem::path& filePath, const matjson::Value& value, const int retries) {
  for (int i = 0; i < retries; ++i) {
    auto res = file::writeString(filePath, value.dump(matjson::NO_INDENTATION));
    if (res.isOk()) {
      return true;
    }

    log::warn("Write failed (attempt {}): {}", i + 1, res.unwrapErr());
  }

  return false;
}

void SaveHandler::saveData() {
  const auto filePath = savePath / (currentLevelID + ".json");
  if (const auto success = tryWrite(filePath, matjson::Value(deaths), 3); !success) {
    log::warn("Failed to save for level ID={}", currentLevelID);
  }
}
