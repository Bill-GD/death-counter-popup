#pragma once

using namespace geode::prelude;

class LeftPanel : public CCScale9Sprite {
protected:
  bool init(const CCSize& size);

public:
  static LeftPanel* create(const CCSize& size);
  void loadLevelList();
};
