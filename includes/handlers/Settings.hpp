#pragma once

class Settings {
  static bool goldenNewBest;
  static bool showForCompleted;

public:
  static void init();

  static void setGoldenNewBest(bool value);
  static void setShowForCompleted(bool value);

  static bool isNewBestGolden();
  static bool isShownForCompleted();
};
