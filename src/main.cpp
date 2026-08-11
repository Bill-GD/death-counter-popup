#include "handlers/DataMigrationHandler.hpp"
#include "handlers/SaveHandler.hpp"
#include "handlers/Settings.hpp"

using namespace geode::prelude;

$execute {
  (void)file::createDirectory(SaveHandler::PATH);
  (void)file::createDirectory(SaveHandler::PATH / "backups");
}

$on_game(Exiting) {
  if (SaveHandler::isLevelSet()) {
    SaveHandler::saveData();
  }
}

$on_mod(Loaded) {
  Settings::init();
  Settings::addListeners();

  DataMigrationHandler::migrateAll();
}
