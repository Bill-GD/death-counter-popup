#pragma once

using namespace geode::prelude;

class LeftPanel : public CCScale9Sprite {
  ScrollLayer* scrollLayer = nullptr;

protected:
  bool init(const CCSize& size);

public:
  static LeftPanel* create(const CCSize& size);
  void loadLevelList(const std::function<void(std::string)>& onSelected) const;
};
