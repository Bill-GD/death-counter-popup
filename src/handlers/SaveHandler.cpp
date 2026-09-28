#include "handlers/SaveHandler.hpp"

#include "handlers/DataMigrationHandler.hpp"
#include "handlers/DeathTrackerHandler.hpp"
#include "utils/FileUtils.hpp"
#include "utils/LevelUtils.hpp"
#include "utils/Utils.hpp"

using namespace geode::prelude;

bool SaveHandler::shouldLoad{};
std::string SaveHandler::currentLevelID{};
std::string SaveHandler::currentLevelName{};
GJLevelType SaveHandler::currentLevelType{};
DeathCounter SaveHandler::deaths{};

const std::string& SaveHandler::getCurrentLevelID() {
  return currentLevelID;
}

bool SaveHandler::isLevelSet() {
  return !currentLevelID.empty();
}

bool SaveHandler::hasLinkedLevel(const std::string& levelID) {
  return std::filesystem::exists(getLevelPath(levelID, SavePathType::LINK));
}

std::string SaveHandler::getLinkedLevel(const std::string& levelID) {
  const auto [success, value] = FileUtils::tryReadString(getLevelPath(levelID, SavePathType::LINK));
  if (!success) return "";
  return value;
}

void SaveHandler::setLevel(GJGameLevel* level) {
  shouldLoad = !level->isPlatformer();
  currentLevelID = LevelUtils::getLevelID(level);
  currentLevelName = level->m_levelName;
  currentLevelType = level->m_levelType;
}

std::filesystem::path SaveHandler::getLevelPath(const std::string& levelID, const SavePathType type) {
  switch (type) {
    case SavePathType::DATA: return PATH / levelID / "data";
    case SavePathType::INFO: return PATH / levelID / "info";
    case SavePathType::LINK: return PATH / levelID / "link";
    case SavePathType::DIR:
    default: return PATH / levelID;
  }
}

bool SaveHandler::pathExists(const std::string& levelID, const SavePathType type) {
  return std::filesystem::exists(getLevelPath(levelID, type));
}

/** @param runKey This key is the most precise variant */
void SaveHandler::incrementRun(const std::string& runKey) {
  if (!shouldLoad) return;

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
        .precision = LevelUtils::getKeyPrecision(key),
        .parent = LevelUtils::getParentKey(key),
      };
    }
    deaths[key].count++;
    key = deaths[key].parent;
  }
  log::info("Logged death/run: {}x{}", runKey, deaths[runKey].count);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID) {
  if (!pathExists(levelID, SavePathType::DATA)) {
    (void)file::createDirectory(PATH / levelID);
    return {};
  }

  const auto [success, val] = FileUtils::tryRead(getLevelPath(levelID, SavePathType::DATA));
  if (!success) return {};

  return Utils::tryParse<DeathCounter>(val);
}

DeathCounter SaveHandler::getLatestLinkedData() {
  const auto linkedLevels = DeathTrackerHandler::getLinkedLevels(currentLevelID);

  std::vector<std::pair<std::string, std::filesystem::file_time_type>> linkedLevelFiles = {};
  for (const auto& linkedLevelID : linkedLevels) {
    if (!pathExists(linkedLevelID, SavePathType::DATA)) continue;
    linkedLevelFiles.emplace_back(
      linkedLevelID,
      std::filesystem::last_write_time(getLevelPath(linkedLevelID, SavePathType::DATA))
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

LevelInfo SaveHandler::getLevelInfo() {
  if (!isLevelSet()) return {};
  return LevelInfo{
    .id = currentLevelID,
    .name = currentLevelName,
  };
}

void SaveHandler::saveCurrentLevelInfo() {
  log::info("Saving info of level {}", currentLevelID);
  FileUtils::tryWrite(getLevelPath(currentLevelID, SavePathType::INFO), matjson::Value(getLevelInfo()));
}

void SaveHandler::loadSaveData() {
  if (!shouldLoad) {
    log::info("This level was marked to not load, possibly platformer");
    deaths = {};
    return;
  }

  log::info("Loading deaths for level '{}' (id={})", currentLevelName, currentLevelID);
  saveCurrentLevelInfo();

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
  if (!shouldLoad) {
    log::info("This level was marked to not save, possibly platformer");
    return;
  }

  saveCurrentLevelInfo();
  if (
    const auto success = FileUtils::tryWrite(getLevelPath(currentLevelID, SavePathType::DATA), matjson::Value(deaths));
    !success
  ) {
    log::warn("Failed to save for level '{}' (id={})", currentLevelName, currentLevelID);
    return;
  }
  log::info("Saved data for level '{}' (id={})", currentLevelName, currentLevelID);
}

LevelInfoFromFileResult SaveHandler::getLevelInfoFromFile(const std::string& levelID, const bool shouldLog) {
  if (!pathExists(levelID, SavePathType::INFO)) return LevelInfoFromFileResult{"", "", ""};

  const auto [levelInfoSuccess, levelInfoValue] = FileUtils::tryRead(
    getLevelPath(levelID, SavePathType::INFO), shouldLog
  );
  if (!levelInfoSuccess) return LevelInfoFromFileResult{"", "", ""};
  const auto [id, name] = Utils::tryParse<LevelInfo>(levelInfoValue);

  const auto [linkLevelSuccess, linkedLevelID] = FileUtils::tryReadString(
    getLevelPath(levelID, SavePathType::LINK), shouldLog
  );
  if (!linkLevelSuccess) return {.id = id, .name = name, .link = ""};

  return LevelInfoFromFileResult{
    .id = id,
    .name = name,
    .link = linkedLevelID,
  };
}
