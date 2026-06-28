#pragma once

using namespace geode::prelude;

class Settings {
  static bool enabled;
  static bool goldenNewBest;
  static bool showForCompleted;
  static int labelXPos;
  static int labelYPos;
  static int rotation;

public:
  static void init();

  static void setEnable(bool value);
  static void setGoldenNewBest(bool value);
  static void setShowForCompleted(bool value);
  static void setLabelXPos(int value);
  static void setLabelYPos(int value);
  static void setRotation(int value);

  static bool isEnabled();
  static bool isNewBestGolden();
  static bool isShownForCompleted();
  static CCPoint getLabelPosition();
  static int getRotation();
};
