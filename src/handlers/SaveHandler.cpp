#include "handlers/SaveHandler.hpp"

#include "handlers/DataMigrationHandler.hpp"
#include "handlers/DeathTrackerHandler.hpp"
#include "handlers/settings/Settings.hpp"
#include "utils/FileUtils.hpp"
#include "utils/LevelUtils.hpp"
#include "utils/Utils.hpp"

using namespace geode::prelude;

std::string SaveHandler::linkedID{};
std::string SaveHandler::currentLevelID{};
std::string SaveHandler::currentLevelName{};
bool SaveHandler::shouldLoad{};

DeathCounter SaveHandler::originalDeathDiff{};
DeathCounter SaveHandler::deaths{};

const std::string& SaveHandler::getCurrentLevelID() {
  return currentLevelID;
}

bool SaveHandler::isLevelSet() {
  return !currentLevelID.empty();
}

bool SaveHandler::hasLinkedLevel(const std::string& levelID) {
  return pathExists(levelID, SavePathType::LINK);
}

std::string SaveHandler::getLinkedLevel(const std::string& levelID) {
  if (!pathExists(levelID, SavePathType::LINK)) return "";
  const auto [success, value] = FileUtils::tryReadString(getLevelPath(levelID, SavePathType::LINK));
  if (!success) return "";
  return value;
}

bool SaveHandler::setLinkedLevel(const std::string& levelID, const std::string& linkToID) {
  if (levelID == currentLevelID) linkedID = linkToID;
  return FileUtils::tryWriteString(getLevelPath(levelID, SavePathType::LINK), linkToID);
}

bool SaveHandler::removeLink(const std::string& levelID) {
  if (levelID == currentLevelID) linkedID = "";
  return FileUtils::tryRemoveFile(getLevelPath(levelID, SavePathType::LINK));
}

void SaveHandler::setLevel(GJGameLevel* level) {
  shouldLoad = !level->isPlatformer();
  currentLevelID = LevelUtils::getLevelID(level);
  currentLevelName = level->m_levelName;
  originalDeathDiff = {};
  linkedID = getLinkedLevel(currentLevelID);
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
  int diffNewRunDataCount = 0;

  // completion, already clamped to 100.00f -> reuse integer variant (if present)
  // only if most precise variant is missing
  if (key.starts_with("100")) {
    if (const auto& it = deaths.find("100"); it != deaths.end()) {
      newRunDataCount = it->second.count;
    }
    if (!linkedID.empty()) {
      if (const auto& it = originalDeathDiff.find("100"); it != originalDeathDiff.end()) {
        diffNewRunDataCount = it->second.count;
      }
    }
  }

  while (!key.empty()) {
    const int keyPrecision = LevelUtils::getKeyPrecision(key);
    const auto parentKey = LevelUtils::getParentKey(key);

    if (!deaths.contains(key)) {
      deaths[key] = RunData{
        .count = newRunDataCount,
        .precision = keyPrecision,
        .parent = parentKey,
      };
    }
    deaths[key].count++;

    if (!linkedID.empty()) {
      if (!originalDeathDiff.contains(key)) {
        originalDeathDiff[key] = RunData{
          .count = diffNewRunDataCount,
          .precision = keyPrecision,
          .parent = parentKey,
        };
      }
      originalDeathDiff[key].count++;
    }

    key = parentKey;
  }
  log::info("Logged death/run: {}x{}", runKey, deaths[runKey].count);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID, const bool getLinked) {
  std::string levelIDToRead;
  if (getLinked && pathExists(levelID, SavePathType::LINK)) {
    levelIDToRead = getLinkedLevel(levelID);
  }

  if (levelIDToRead.empty()) levelIDToRead = levelID;
  if (!pathExists(levelIDToRead, SavePathType::DATA)) {
    (void)file::createDirectory(getLevelPath(levelIDToRead, SavePathType::DIR));
    return {};
  }

  const auto [success, val] = FileUtils::tryRead(getLevelPath(levelIDToRead, SavePathType::DATA));
  if (!success) return {};

  return Utils::tryParse<DeathCounter>(val);
}

void SaveHandler::mergeDTData(const std::string& levelID) {
  auto levelData = getSavedData(levelID, true);

  auto otherData = getLatestLinkedData(levelID);
  if (otherData.empty()) {
    // log::info("No linked level found, load from Death Tracker");
    otherData = DataMigrationHandler::parseOldData(DeathTrackerHandler::getSaveData(levelID));
  }

  if (otherData.empty()) return;
  bool changed = false;
  for (const auto& [runKey, runData] : otherData) {
    if (!levelData.contains(runKey) || levelData[runKey].count < runData.count) {
      levelData[runKey] = runData;
      changed = true;
    }
  }
  if (changed) {
    // log::info("Updated save data");
    const auto linkedParent = getLinkedLevel(levelID);
    saveData(linkedParent.empty() ? levelID : linkedParent, levelData);
  }
}

DeathCounter SaveHandler::getLatestLinkedData(const std::string& levelID) {
  const auto linkedLevels = DeathTrackerHandler::getLinkedLevels(levelID);

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
  return getSavedData(linkedLevelFiles[0].first, false);
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

void SaveHandler::loadSaveData(bool skipDTMerge) {
  originalDeathDiff = {};
  if (!shouldLoad) {
    log::info("This level was marked to not load, possibly platformer");
    deaths = {};
    return;
  }

  log::info("Loading deaths for level '{}' (id={})", currentLevelName, currentLevelID);
  deaths = getSavedData(currentLevelID, !linkedID.empty());

  if (!skipDTMerge && Settings::isDTAutoMerge()) {
    auto otherData = getLatestLinkedData(currentLevelID);
    if (otherData.empty()) {
      log::info("No linked level found, load from Death Tracker");
      otherData = DataMigrationHandler::parseOldData(DeathTrackerHandler::getSaveData(currentLevelID));
    }

    if (!otherData.empty()) {
      bool changed = false;
      for (const auto& [runKey, runData] : otherData) {
        if (!deaths.contains(runKey) || deaths[runKey].count < runData.count) {
          deaths[runKey] = runData;
          changed = true;
        }
      }
      if (changed) {
        log::info("Updated save data");
        const auto linkedParent = getLinkedLevel(currentLevelID);
        saveData(linkedParent.empty() ? currentLevelID : linkedParent, deaths);
      }
    }
  }
  saveCurrentLevelInfo();
}

// Apply diff to original data of child level (only when linked)
void SaveHandler::saveDiff() {
  if (linkedID.empty() || originalDeathDiff.empty()) return;

  auto data = getSavedData(currentLevelID, false);

  bool changed = false;
  for (const auto& [runKey, runData] : originalDeathDiff) {
    if (!data.contains(runKey)) {
      data[runKey] = runData;
      changed = true;
    } else {
      data[runKey].count += runData.count;
      changed = true;
    }
  }
  if (changed) {
    log::info("Applied diff to original save data");
    saveData(currentLevelID, data);
  }
  originalDeathDiff = {};
}

// Save data for currently loaded level
void SaveHandler::saveData() {
  if (!shouldLoad) {
    log::info("This level was marked to not save, possibly platformer");
    return;
  }
  saveCurrentLevelInfo();

  const auto pathToSave = getLevelPath(linkedID.empty() ? currentLevelID : linkedID, SavePathType::DATA);
  const auto success = FileUtils::tryWrite(pathToSave, matjson::Value(deaths));

  if (linkedID.empty()) {
    if (success) {
      log::info("Saved data for level '{}' (id={})", currentLevelName, currentLevelID);
    } else {
      log::warn("Failed to save for level '{}' (id={})", currentLevelName, currentLevelID);
    }
  } else {
    if (success) {
      log::info("Saved data to linked level: '{}' (id={}) -> id={}", currentLevelName, currentLevelID, linkedID);
    } else {
      log::warn("Failed to save to linked level: '{}' (id={}) -> id={}", currentLevelName, currentLevelID, linkedID);
    }
    saveDiff();
  }
}

// Save data for specified level ID, must pass in data
void SaveHandler::saveData(const std::string& levelID, const DeathCounter& data) {
  if (
    const auto success = FileUtils::tryWrite(getLevelPath(levelID, SavePathType::DATA), matjson::Value(data));
    !success
  ) {
    log::warn("Failed to save for level '{}' (id={})", currentLevelName, levelID);
    return;
  }
  log::info("Saved data for level '{}' (id={})", currentLevelName, levelID);
}

bool SaveHandler::deleteSavedData(const std::string& levelID) {
  if (levelID.empty() || !pathExists(levelID, SavePathType::DIR)) return false;

  const auto path = getLevelPath(levelID, SavePathType::DIR);
  const auto success = FileUtils::tryRemoveDirectory(path);
  if (success) {
    log::info("Deleted saved data of {}", levelID);
  } else {
    log::info("Failed to delete saved data of {}", levelID);
  }
  return success;
}

LevelInfoFromFileResult SaveHandler::getLevelInfoFromFile(const std::string& levelID, const bool shouldLog) {
  if (!pathExists(levelID, SavePathType::INFO)) return LevelInfoFromFileResult{"", "", ""};

  const auto [levelInfoSuccess, levelInfoValue] = FileUtils::tryRead(
    getLevelPath(levelID, SavePathType::INFO), shouldLog
  );
  if (!levelInfoSuccess) return LevelInfoFromFileResult{"", "", ""};
  const auto [id, name] = Utils::tryParse<LevelInfo>(levelInfoValue);

  if (!pathExists(levelID, SavePathType::LINK)) return LevelInfoFromFileResult{id, name, ""};
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

LevelSaveDataMetadata SaveHandler::getMetadata(const std::string& levelID) {
  if (!pathExists(levelID, SavePathType::DIR) || !pathExists(levelID, SavePathType::DATA)) return {0, {}};
  const auto size = FileUtils::getDirectorySize(getLevelPath(levelID, SavePathType::DIR));
  const auto lastModifiedTime = FileUtils::getFileLastWriteTime(getLevelPath(levelID, SavePathType::DATA));
  return {size, lastModifiedTime};
}
