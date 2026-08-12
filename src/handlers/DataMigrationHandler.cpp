#include "handlers/DataMigrationHandler.hpp"

#include "handlers/SaveHandler.hpp"
#include "utils/FileUtils.hpp"
#include "utils/LevelUtils.hpp"
#include "utils/Utils.hpp"

std::map<RunKey, RunData> DataMigrationHandler::parseOldData(const OldDeathCounter& oldData) {
  std::map<RunKey, RunData> newData{};

  // parse all the old runs into new format
  for (const auto& [key, count] : oldData) {
    const auto& [newKey, parentKey] = LevelUtils::computeRunKeys(key);
    newData[newKey] = RunData{
      .count = count,
      .parent = parentKey,
    };
  }

  // add RunData for all missing parents
  std::vector<std::string> keys;
  keys.reserve(newData.size());
  for (const auto& [k, _] : newData) {
    keys.push_back(k);
  }

  for (int i = 0; i < keys.size(); ++i) {
    const auto key = keys[i];
    const auto& runDataIt = newData.find(key);
    if (runDataIt == newData.end()) continue;

    const auto& [count, parentKey] = runDataIt->second;
    if (parentKey.empty()) continue;

    if (auto parentIt = newData.find(parentKey); parentIt != newData.end()) {
      parentIt->second.count += count;
    } else {
      newData.emplace(
        parentKey,
        RunData{
          .count = count,
          .parent = LevelUtils::getParentKey(parentKey),
        }
      );
      keys.push_back(parentKey);
    }
  }

  return newData;
}

void DataMigrationHandler::migrateLevel(const std::string& levelID) {
  const auto& oldPath = SaveHandler::PATH / (levelID + ".json");
  const auto& newPath = SaveHandler::PATH / levelID;
  // ignore linking for now
  // const auto linkedLevels = DeathTrackerHandler::getLinkedLevels(levelID);

  auto [readSuccess, val] = FileUtils::tryRead(oldPath);
  if (!readSuccess) {
    log::warn("Failed to read save data of '{}'", levelID);
    return;
  }

  if (const auto res = file::createDirectory(newPath); res.isErr()) {
    log::warn("Failed to create new directory for '{}'", levelID);
    return;
  }

  const auto oldData = Utils::tryParse<OldDeathCounter>(val);
  const auto writeSuccess = FileUtils::tryWrite(newPath / "data", parseOldData(oldData));
  log::info("Migration of level '{}': {}", levelID, writeSuccess ? "Success" : "Fail");

  const auto& backupPath = SaveHandler::PATH / "backups" / (levelID + ".json");
  if (const auto success = FileUtils::tryMove(oldPath, backupPath); !success) {
    log::info("Move to backup failed, file will be left in place");
  }
}

bool DataMigrationHandler::migrateAll() {
  const auto files = FileUtils::getAllFiles(SaveHandler::PATH);
  const std::vector<std::filesystem::path> jsonFiles = ranges::filter(
    files,
    [](auto const& file) {
      return file.extension().string() == ".json";
    }
  );

  if (jsonFiles.empty()) {
    log::info("No files left to migrate in '{}'", SaveHandler::PATH.string());
    return false;
  }

  for (const auto& filePath : jsonFiles) {
    const auto levelID = filePath.stem().string();
    migrateLevel(levelID);
  }
  return true;
}
