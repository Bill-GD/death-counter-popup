#pragma once

using namespace geode::prelude;

class RightPanel : public CCScale9Sprite {
protected:
  bool init() override;

public:
  static RightPanel* create();
  void loadLevel(std::string levelID);
};
