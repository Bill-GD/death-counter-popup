#pragma once

#include "types/Run.hpp"
#include "types/types.hpp"

using namespace geode::prelude;

class DataMigrationHandler {
public:
  static std::map<RunKey, RunData> parseOldData(const OldDeathCounter& oldData);
  static void migrateLevel(const std::string& levelID);
  static void migrateAll();
};
