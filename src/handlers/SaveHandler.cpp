#include <handlers/DeathTrackerHandler.hpp>
#include <handlers/SaveHandler.hpp>
#include <utils/Utils.hpp>

using namespace geode::prelude;

std::string SaveHandler::currentLevelID{};
std::string SaveHandler::currentLevelName{};
DeathCounter SaveHandler::deaths{};

void SaveHandler::setLevel(GJGameLevel* level) {
  currentLevelID = Utils::getLevelID(level);
  currentLevelName = level->m_levelName;
}

std::filesystem::path SaveHandler::getLevelPath(const std::string& levelID) {
  return PATH / (levelID + ".json");
}

bool SaveHandler::isSaveExists(const std::string& levelID) {
  return std::filesystem::exists(getLevelPath(levelID));
}

void SaveHandler::updateDeath(const std::string& death) {
  deaths[death]++;
  log::info("Logged death/run: {}x{}", death, deaths[death]);
}

DeathCounter SaveHandler::getSavedData(const std::string& levelID) {
  if (!isSaveExists(levelID)) return {};

  const auto [success, val] = Utils::tryRead(getLevelPath(levelID));
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
  log::info("Loading deaths for level {} (id={})", currentLevelName, currentLevelID);

  auto otherData = getLatestLinkedData();
  if (otherData.empty()) {
    log::info("No linked level found, load from Death Tracker");
    otherData = DeathTrackerHandler::getSaveData(currentLevelID);
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

void SaveHandler::saveData() {
  if (
    const auto success = Utils::tryWrite(getLevelPath(currentLevelID), matjson::Value(deaths));
    !success
  ) {
    log::warn("Failed to save for level {} (id={})", currentLevelName, currentLevelID);
  }
  log::info("Saved data for level {} (id={})", currentLevelName, currentLevelID);
}
