#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(DCPPlayLayer, PlayLayer) {
  struct Fields {
    CCSize winSize;
    CCLabelBMFont* label = nullptr;
    int runStartPercent = 0;
    bool isNoclipping = false;
    CCObject* currentAttemptGameObject = nullptr;
    Fields() : winSize(CCDirector::get()->getWinSize()) {}
  };

  bool init(GJGameLevel* level, bool useReplay, bool dontCreateObject);
  void onExit() override;
  void resetLevel() override;
  void destroyPlayer(PlayerObject* player, GameObject* gameObject) override;

  void removeLabel();
  CCLabelBMFont* getPopupLabel(std::string deathKey);
};
