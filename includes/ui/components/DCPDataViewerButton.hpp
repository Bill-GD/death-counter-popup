#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class DCPDataViewerButton : public CCMenuItemSpriteExtra {
  void onClick(CCObject*);

protected:
  bool init(float scale);

public:
  static DCPDataViewerButton* create(float scale);
};
