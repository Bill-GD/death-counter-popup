#pragma once

using namespace geode::prelude;

class RightPanel : public CCNode {
  Label* messageLabel = nullptr;
  CCNode* infoTextContainer = nullptr;
  CCScale9Sprite* infoRegion = nullptr;

protected:
  bool init(float width, float controlHeight, float infoHeight, float gap);

public:
  static RightPanel* create(float width, float controlHeight, float infoHeight, float gap);
  void loadLevelInfo(std::string levelID);
};
