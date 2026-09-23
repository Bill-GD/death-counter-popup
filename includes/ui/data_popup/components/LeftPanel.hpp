#pragma once
#include "types/LevelInfo.hpp"

using namespace geode::prelude;

class LeftPanel : public CCNode {
  std::vector<std::pair<std::string, LevelInfo>> allLevels = {};
  std::vector<std::pair<std::string, LevelInfo>> filteredLevels = {};
  ScrollLayer* scrollLayer = nullptr;
  Label* countLabel = nullptr;
  std::string pendingFilterInput;
  std::function<void(std::string)> onSelectedCallback = nullptr;

  void onInputChanged(const std::string& value);

protected:
  bool init(float width, float controlHeight, float listHeight, float gap);

public:
  static LeftPanel* create(float width, float controlHeight, float listHeight, float gap);
  void loadLevelList();
  void executeFiltering();
  void filterLevels(const std::string& input);
  void setOnSelectedCallback(const std::function<void(std::string)>& onSelected);
  void displayLevelList() const;
};
