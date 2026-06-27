#include <handlers/SaveHandler.hpp>
#include <handlers/Settings.hpp>

using namespace geode::prelude;

$execute {
  (void)file::createDirectory(SaveHandler::savePath);
}

$on_game(Exiting) {
  SaveHandler::saveData();
}

$on_mod(Loaded) {
  Settings::init();

  listenForSettingChanges<bool>(
    "gold-new-best",
    [](const bool value) {
      Settings::setGoldenNewBest(value);
    }
  );

  listenForSettingChanges<bool>(
    "show-for-completed",
    [](const bool value) {
      Settings::setShowForCompleted(value);
    }
  );
}
