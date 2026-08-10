#include "handlers/DataMigrationHandler.hpp"

#include "handlers/SaveHandler.hpp"
#include "utils/FileUtils.hpp"
#include "utils/Utils.hpp"

void DataMigrationHandler::migrateLevelData(const std::string& levelID) {
  const auto& oldPath = SaveHandler::PATH / (levelID + ".json");
  const auto& newPath = SaveHandler::PATH / levelID;
  // ignore linking for now
  // const auto linkedLevels = DeathTrackerHandler::getLinkedLevels(levelID);

  auto [readSuccess, val] = FileUtils::tryRead(oldPath);
  if (!readSuccess) {
    log::warn("Failed to read save data of '{}'", levelID);
    return;
  }

  const auto oldData = Utils::tryParse<OldDeathCounter>(val);

  if (const auto res = file::createDirectory(newPath); res.isErr()) {
    log::warn("Failed to create new directory for '{}'", levelID);
    return;
  }

  std::map<RunKey, DeathNodeData> levelDeathData{};
  for (const auto& [key, count] : oldData) {
    const auto& [newKey, parentKey] = Utils::computeRunKeys(key);
    levelDeathData[newKey] = DeathNodeData{
      .count = count,
      .parent = parentKey,
    };
  }

  std::vector<std::string> keys;
  keys.reserve(levelDeathData.size());
  for (const auto& [k, _] : levelDeathData) {
    keys.push_back(k);
  }

  for (int i = 0; i < keys.size(); ++i) {
    const auto key = keys[i];
    const auto& deathDataIt = levelDeathData.find(key);
    if (deathDataIt == levelDeathData.end()) continue;

    const auto& [count, parentKey] = deathDataIt->second;
    if (parentKey.empty()) continue;

    if (auto parentIt = levelDeathData.find(parentKey); parentIt != levelDeathData.end()) {
      parentIt->second.count += count;
    } else {
      levelDeathData.emplace(
        parentKey,
        DeathNodeData{
          .count = count,
          .parent = Utils::computeRunKeys(parentKey).second,
        }
      );
      keys.push_back(parentKey);
    }
  }

  const auto writeSuccess = FileUtils::tryWrite(newPath / "data", levelDeathData);
  log::info("Migration of '{}': {}", levelID, writeSuccess ? "Success" : "Fail");
  // delete old file
}
