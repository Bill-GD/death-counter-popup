#pragma once

using namespace geode::prelude;

class DCPLevelPopup : public Popup {
protected:
  bool init(const GJGameLevel& level);

public:
  static DCPLevelPopup* create(const GJGameLevel& level);
};
