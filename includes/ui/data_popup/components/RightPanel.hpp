#pragma once

using namespace geode::prelude;

class RightPanel : public CCScale9Sprite {
  Label* messageLabel = nullptr;
  CCNode* infoContainer = nullptr;

protected:
  bool init(const CCSize& size);

public:
  static RightPanel* create(const CCSize& size);
  void loadLevelInfo(std::string levelID);
};
