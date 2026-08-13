#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class Settings {
  static bool enabled;
  static bool goldenNewBest;
  static bool showForCompleted;
  static int labelPrecision;
  static int labelXPos;
  static int labelYPos;
  static int rotation;
  static float scale;
  static float opacity;

public:
  static void init();
  static void addListeners();

  static void setEnable(bool value);
  static void setGoldenNewBest(bool value);
  static void setShowForCompleted(bool value);
  static void setLabelPrecision(int value);
  static void setLabelXPos(int value);
  static void setLabelYPos(int value);
  static void setRotation(int value);
  static void setScale(float value);
  static void setOpacity(float value);

  static bool isEnabled();
  static bool isNewBestGolden();
  static bool isShownForCompleted();
  static int getLabelPrecision();
  static CCPoint getLabelPosition();
  static int getRotation();
  static float getScale();
  static GLubyte getOpacity();
};
