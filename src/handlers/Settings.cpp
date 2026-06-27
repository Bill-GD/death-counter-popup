#include <handlers/Settings.hpp>

using namespace geode::prelude;

bool Settings::goldenNewBest{};
bool Settings::showForCompleted{};

void Settings::init() {
  goldenNewBest = Mod::get()->getSettingValue<bool>("gold-new-best");
  showForCompleted = Mod::get()->getSettingValue<bool>("show-for-completed");
}

void Settings::setGoldenNewBest(const bool value) {
  goldenNewBest = value;
}

void Settings::setShowForCompleted(const bool value) {
  showForCompleted = value;
}

bool Settings::isNewBestGolden() {
  return goldenNewBest;
}

bool Settings::isShownForCompleted() {
  return showForCompleted;
}
