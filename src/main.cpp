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

  listenForSettingChanges<bool>("enable", Settings::setEnable);
  listenForSettingChanges<bool>("gold-new-best", Settings::setGoldenNewBest);
  listenForSettingChanges<bool>("show-for-completed", Settings::setShowForCompleted);
  listenForSettingChanges<int>("x-position", Settings::setLabelXPos);
  listenForSettingChanges<int>("y-position", Settings::setLabelYPos);
  listenForSettingChanges<int>("rotation", Settings::setRotation);
  listenForSettingChanges<float>("scale", Settings::setScale);
}
