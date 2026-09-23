#pragma once

using namespace geode::prelude;

class RightPanel : public CCNode {
  std::string selectedLevelID;
  Label* infoLabel = nullptr;

protected:
  bool init(float width, float controlHeight, float infoHeight, float gap);

public:
  static RightPanel* create(float width, float controlHeight, float infoHeight, float gap);
  void loadLevelInfo(std::string levelID);
};
