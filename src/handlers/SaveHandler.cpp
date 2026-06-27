#include <handlers/SaveHandler.hpp>
#include <utils/Utils.hpp>

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
  if (!Utils::isModLoaded("elohmrow.death_tracker")) return {};

  const auto filePath = dtPath / levelID / DT_METADATA_FILENAME;

  if (!isDTSaveExists(levelID)) return {};

  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  const auto& json = readRes.unwrap();
  return json["LinkedLevels"].as<std::set<std::string>>().unwrap();
}

DeathCounter SaveHandler::getDTDeaths(const std::string& levelID) {
  const auto filePath = dtPath / levelID / DT_LEVEL_GENERAL_FILENAME;
  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  const auto& json = readRes.unwrap();

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
  log::info("Loading deaths for level ID={}", currentLevelID);
  const auto linkedLevels = getDTLinkedLevels(currentLevelID);

  std::vector<std::pair<std::string, std::filesystem::file_time_type>> linkedLevelFiles = {};
  for (const auto& linkedLevelID : linkedLevels) {
    if (!isSaveExists(linkedLevelID)) continue;
    const auto filePath = savePath / (linkedLevelID + ".json");
    linkedLevelFiles.emplace_back(linkedLevelID, std::filesystem::last_write_time(filePath));
  }

  if (!linkedLevelFiles.empty()) {
    std::ranges::sort(
      linkedLevelFiles,
      [](auto const& a, auto const& b) { return a.second > b.second; }
    );
    log::info("Use own save data of last modified linked level, chosen ID={}", linkedLevelFiles[0].first);
    deaths = getSavedData(linkedLevelFiles[0].first);
  } else {
    log::info("No save data found, load from Death Tracker");
    deaths = getDTSaveData(currentLevelID);
  }

  if (deaths.empty()) {
    log::info("No other save data found, use own save data");
    deaths = getSavedData(currentLevelID);
  }
  saveData();
}

void SaveHandler::updateDeath(const std::string& death) {
  deaths[death]++;
  log::info("Logged death/run: {}x{}", death, deaths[death]);
}

bool SaveHandler::tryWrite(const std::filesystem::path& filePath, const matjson::Value& value) {
  for (int i = 0; i < 3; ++i) {
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
  if (const auto success = tryWrite(filePath, matjson::Value(deaths)); !success) {
    log::warn("Failed to save for level ID={}", currentLevelID);
  }
}
