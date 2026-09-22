#pragma once

using namespace geode::prelude;

class LevelTile : public CCNode {
  std::function<void(std::string)> onLevelSelected;

  void onTileClicked(CCObject* sender) const;

protected:
  bool init(std::string levelID, const CCSize& size, std::function<void(std::string)> onSelected);

public:
  static LevelTile* create(const std::string& levelID, const CCSize& size, const std::function<void(std::string)>& onSelected);
};
