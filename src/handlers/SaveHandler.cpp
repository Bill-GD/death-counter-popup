#include "handlers/SaveHandler.hpp"

#include "handlers/DataMigrationHandler.hpp"
#include "handlers/DeathTrackerHandler.hpp"
#include "utils/FileUtils.hpp"
#include "utils/LevelUtils.hpp"
#include "utils/Utils.hpp"

using namespace geode::prelude;

std::string SaveHandler::currentLevelID{};
std::string SaveHandler::currentLevelName{};
DeathCounter SaveHandler::deaths{};

bool SaveHandler::isLevelSet() {
  return !currentLevelID.empty();
}

void SaveHandler::setLevel(GJGameLevel* level) {
  currentLevelID = LevelUtils::getLevelID(level);
  currentLevelName = level->m_levelName;
}

std::filesystem::path SaveHandler::getLevelPath(const std::string& levelID) {
  return PATH / levelID / "data";
}

bool SaveHandler::isSaveExists(const std::string& levelID) {
  return std::filesystem::exists(getLevelPath(levelID));
}

/** @param runKey This key is the most precise variant */
void SaveHandler::incrementRun(const std::string& runKey) {
  auto key = runKey;
  int newRunDataCount = 0;

  // completion, already clamped to 100.00f -> reuse integer variant (if present)
  // only if most precise variant is missing
  if (key.starts_with("100")) {
    if (const auto& it = deaths.find("100"); it != deaths.end()) {
      newRunDataCount = it->second.count;
    }
  }

  while (!key.empty()) {
    if (!deaths.contains(key)) {
      deaths[key] = RunData{
        .count = newRunDataCount,
        .parent = LevelUtils::getParentKey(key),
      };
    }
    deaths[key].count++;
    key = deaths[key].parent;
  }
  log::info("Logged death/run: {}x{}", runKey, deaths[runKey].count);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID) {
  if (!isSaveExists(levelID)) return {};

  const auto [success, val] = FileUtils::tryRead(getLevelPath(levelID));
  if (!success) return {};

  return Utils::tryParse<DeathCounter>(val);
}

DeathCounter SaveHandler::getLatestLinkedData() {
  const auto linkedLevels = DeathTrackerHandler::getLinkedLevels(currentLevelID);

  std::vector<std::pair<std::string, std::filesystem::file_time_type>> linkedLevelFiles = {};
  for (const auto& linkedLevelID : linkedLevels) {
    if (!isSaveExists(linkedLevelID)) continue;
    linkedLevelFiles.emplace_back(
      linkedLevelID,
      std::filesystem::last_write_time(getLevelPath(linkedLevelID))
    );
  }

  log::info("Linked level file found: {}", linkedLevelFiles.size());
  if (linkedLevelFiles.empty()) return {};

  std::ranges::sort(
    linkedLevelFiles,
    [](auto const& a, auto const& b) { return a.second > b.second; }
  );

  log::info(
    "Got save data of last modified linked level, chosen (id={})",
    linkedLevelFiles[0].first
  );
  return getSavedData(linkedLevelFiles[0].first);
}

void SaveHandler::loadSaveData() {
  log::info("Loading deaths for level '{}' (id={})", currentLevelName, currentLevelID);

  auto otherData = getLatestLinkedData();
  if (otherData.empty()) {
    log::info("No linked level found, load from Death Tracker");
    otherData = DataMigrationHandler::parseOldData(DeathTrackerHandler::getSaveData(currentLevelID));
  }

  deaths = getSavedData(currentLevelID);
  if (otherData.empty()) return;

  bool changed = false;
  for (const auto& [runKey, runData] : otherData) {
    if (!deaths.contains(runKey) || deaths[runKey].count < runData.count) {
      deaths[runKey] = runData;
      changed = true;
    }
  }
  if (changed) {
    log::info("Updated save data");
    saveData();
  }
}

void SaveHandler::saveData() {
  if (
    const auto success = FileUtils::tryWrite(getLevelPath(currentLevelID), matjson::Value(deaths));
    !success
  ) {
    log::warn("Failed to save for level '{}' (id={})", currentLevelName, currentLevelID);
  }
  log::info("Saved data for level '{}' (id={})", currentLevelName, currentLevelID);
}
