#pragma once

using namespace geode::prelude;

class LeftPanel : public CCScale9Sprite {
  std::vector<std::string> allLevelIDs = {};
  std::vector<std::string> filteredLevelIDs = {};
  ScrollLayer* scrollLayer = nullptr;

protected:
  bool init(const CCSize& size);

public:
  static LeftPanel* create(const CCSize& size);
  void loadLevelList();
  void filterByName(std::string input);
  void displayLevelList(const std::function<void(std::string)>& onSelected) const;
};
