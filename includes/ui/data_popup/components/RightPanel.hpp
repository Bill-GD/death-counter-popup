#pragma once

using namespace geode::prelude;

class RightPanel : public CCScale9Sprite {
protected:
  bool init(const CCSize& size);

public:
  static RightPanel* create(const CCSize& size);
  void loadLevelInfo(std::string levelID);
};
