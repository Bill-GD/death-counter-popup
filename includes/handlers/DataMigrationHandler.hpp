#pragma once

using namespace geode::prelude;

class DataMigrationHandler {
public:
  static void migrateLevelData(const std::string& levelID);
};
