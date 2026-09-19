#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

inline constexpr std::string_view POPUP_STYLE_ANIMATED = "animated";
inline constexpr std::string_view POPUP_STYLE_FADE = "fade";
inline constexpr std::string_view POPUP_STYLE_FLAT = "flat";

#include <Geode/modify/PlayLayer.hpp>
class $modify(DCPPlayLayer, PlayLayer) {
  struct Fields {
    CCNode* label = nullptr;
    CCLabelBMFont* bestLabel = nullptr;

    float runStartPercent = 0;
    int currentBest = 0;

    const float endScale = 0.65f;

    bool isNoclipping = false;
    CCObject* currentAttemptGameObject = nullptr;
    Fields() {}
  };

  bool init(GJGameLevel* level, bool useReplay, bool dontCreateObject);
  void onQuit();
  void resetLevel() override;
  void destroyPlayer(PlayerObject* player, GameObject* gameObject) override;
  void levelComplete();
  void updateProgressbar();

  void removeLabel();
  CCNode* getPopupLabel(const std::string& deathKey);
  void spawnLabel(const std::string& labelStr);
  CCSequence* getPopupSequence(bool isParent);
  std::string getRunLabelString(const float& currentPercent, const float& maxClamp = 100.f);

  void fetchBestLabel();
  void updateBestPercentageLabel();
  static float getActualCurrentPercent();
};
