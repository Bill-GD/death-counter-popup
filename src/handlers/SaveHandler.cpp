#include <handlers/SaveHandler.hpp>
#include <utils/Utils.hpp>

using namespace geode::prelude;

std::string SaveHandler::currentLevelID{};
DeathCounter SaveHandler::deaths{};

bool SaveHandler::isDTSaveExists() {
  const auto dirPath = dtPath / currentLevelID;
  return std::filesystem::exists(dirPath) && std::filesystem::exists(dirPath / DT_METADATA_FILENAME);
}

std::set<std::string> SaveHandler::getDTLinkedLevels() {
  if (!Utils::isModLoaded("elohmrow.death_tracker") || !isDTSaveExists()) return {};

  const auto filePath = dtPath / currentLevelID / DT_METADATA_FILENAME;

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
    for (
      const auto levelDeaths = getDTDeaths(levelID);
      const auto& [key, value] : levelDeaths
    ) {
      result[key] += value;
    }
  }
  return result;
}

DeathCounter SaveHandler::getDTSaveData() {
  auto linkedLevels = getDTLinkedLevels();
  linkedLevels.insert(currentLevelID);
  return mergeDTDeaths(linkedLevels);
}

bool SaveHandler::isSaveExists() {
  const auto filePath = savePath / (currentLevelID + ".json");
  return std::filesystem::exists(filePath);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID) {
  if (!isSaveExists()) return {};

  const auto filePath = savePath / (levelID + ".json");
  auto readRes = file::readJson(filePath);
  if (readRes.isErr()) return {};

  return readRes.unwrap().as<DeathCounter>().unwrap();
}

DeathCounter SaveHandler::getLatestLinkedData() {
  const auto linkedLevels = getDTLinkedLevels();

  std::vector<std::pair<std::string, std::filesystem::file_time_type>> linkedLevelFiles = {};
  for (const auto& linkedLevelID : linkedLevels) {
    if (!isSaveExists()) continue;
    const auto filePath = savePath / (linkedLevelID + ".json");
    linkedLevelFiles.emplace_back(linkedLevelID, std::filesystem::last_write_time(filePath));
  }

  log::info("Linked level file found: {}", linkedLevelFiles.size());
  if (linkedLevelFiles.empty()) return {};

  std::ranges::sort(
    linkedLevelFiles,
    [](auto const& a, auto const& b) { return a.second > b.second; }
  );

  log::info("Got save data of last modified linked level, chosen ID={}", linkedLevelFiles[0].first);
  return getSavedData(linkedLevelFiles[0].first);
}

void SaveHandler::loadSaveData() {
  log::info("Loading deaths for level ID={}", currentLevelID);

  auto otherData = getLatestLinkedData();
  if (otherData.empty()) {
    log::info("No linked level found, load from Death Tracker");
    otherData = getDTSaveData();
  }

  deaths = getSavedData(currentLevelID);
  if (!otherData.empty()) {
    bool changed = false;
    for (const auto& [run, count] : otherData) {
      if (!deaths.contains(run) || deaths[run] < count) {
        deaths[run] = count;
        changed = true;
      }
    }
    if (changed) {
      log::info("Updated save data");
      saveData();
    }
  }
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
  log::info("Saved data for level ID={}", currentLevelID);
}
