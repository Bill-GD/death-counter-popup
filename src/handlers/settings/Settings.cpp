#include "handlers/settings/Settings.hpp"

using namespace geode::prelude;

bool Settings::enabled{};
bool Settings::goldenNewBest{};
bool Settings::showForCompleted{};
int Settings::labelPrecision{};
int Settings::labelXPos{};
int Settings::labelYPos{};
int Settings::rotation{};
float Settings::scale{};

void Settings::init() {
  enabled = Mod::get()->getSettingValue<bool>("enable");
  goldenNewBest = Mod::get()->getSettingValue<bool>("gold-new-best");
  showForCompleted = Mod::get()->getSettingValue<bool>("show-for-completed");
  labelPrecision = Mod::get()->getSettingValue<int>("label-precision");
  labelXPos = Mod::get()->getSettingValue<int>("x-position");
  labelYPos = Mod::get()->getSettingValue<int>("y-position");
  rotation = Mod::get()->getSettingValue<int>("rotation");
  scale = Mod::get()->getSettingValue<float>("scale");
}

void Settings::addListeners() {
  listenForSettingChanges<bool>("enable", setEnable);
  listenForSettingChanges<bool>("gold-new-best", setGoldenNewBest);
  listenForSettingChanges<bool>("show-for-completed", setShowForCompleted);
  listenForSettingChanges<int>("label-precision", setLabelPrecision);
  listenForSettingChanges<int>("x-position", setLabelXPos);
  listenForSettingChanges<int>("y-position", setLabelYPos);
  listenForSettingChanges<int>("rotation", setRotation);
  listenForSettingChanges<float>("scale", setScale);

  ButtonSettingPressedEventV3(Mod::get(), "test-actions").listen(
    [](auto buttonKey) {
      if (buttonKey == "test-button-1") {} else if (buttonKey == "test-button-2") {}
    }
  ).leak();
}

void Settings::setEnable(const bool value) { enabled = value; }
void Settings::setGoldenNewBest(const bool value) { goldenNewBest = value; }
void Settings::setShowForCompleted(const bool value) { showForCompleted = value; }
void Settings::setLabelPrecision(const int value) { labelPrecision = value; }
void Settings::setLabelXPos(const int value) { labelXPos = value; }
void Settings::setLabelYPos(const int value) { labelYPos = value; }
void Settings::setRotation(const int value) { rotation = value; }
void Settings::setScale(const float value) { scale = value; }

bool Settings::isEnabled() { return enabled; }
bool Settings::isNewBestGolden() { return goldenNewBest; }
bool Settings::isShownForCompleted() { return showForCompleted; }
int Settings::getLabelPrecision() { return labelPrecision; }
CCPoint Settings::getLabelPosition() { return {static_cast<float>(labelXPos), static_cast<float>(labelYPos)}; }
int Settings::getRotation() { return rotation; }
float Settings::getScale() { return scale; }
