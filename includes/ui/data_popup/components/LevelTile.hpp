#pragma once
#include "types/types.hpp"

using namespace geode::prelude;

class LevelTile : public CCNode {
  std::function<void(std::string)> onLevelSelected;

  void onTileClicked(CCObject* sender) const;

protected:
  bool init(LevelTileInfo tileInfo, const CCSize& size, std::function<void(std::string)> onSelected);

public:
  static LevelTile* create(
    const LevelTileInfo& tileInfo,
    const CCSize& size,
    const std::function<void(std::string)>& onSelected
  );
};
