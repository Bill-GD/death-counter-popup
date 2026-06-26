#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(DCPPlayLayer, PlayLayer) {
  struct Fields {
    CCSize winSize;
    CCLabelBMFont* label = nullptr;
    int runStartPercent = 0;
    Fields() : winSize(CCDirector::get()->getWinSize()) {}
  };

  void resetLevel() override;
  void destroyPlayer(PlayerObject* player, GameObject* object) override;

  void removeLabel();
  CCLabelBMFont* getPopupLabel();
};
