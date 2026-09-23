#pragma once

using namespace geode::prelude;

class LeftPanel : public CCNode {
  std::vector<std::string> allLevelIDs = {};
  std::vector<std::string> filteredLevelIDs = {};
  ScrollLayer* scrollLayer = nullptr;
  Label* countLabel = nullptr;

  void onInputChanged(const std::string& value);

protected:
  bool init(float width, float controlHeight, float listHeight, float gap);

public:
  static LeftPanel* create(float width, float controlHeight, float listHeight, float gap);
  void loadLevelList();
  void filterByName(std::string input);
  void displayLevelList(const std::function<void(std::string)>& onSelected) const;
};
