#pragma once

#include "types/LevelInfo.hpp"

using namespace geode::prelude;

class LeftPanel : public CCNode {
  using LoadedData = std::vector<std::pair<std::string, LevelInfo>>;

  LoadedData m_allLevels = {};
  LoadedData m_filteredLevels = {};
  ScrollLayer* m_scrollLayer = nullptr;
  Label* m_countLabel = nullptr;
  std::string m_filterInput;
  std::function<void(std::string)> m_onSelectedCallback = nullptr;
  LoadingSpinner* m_loadingSpinner = nullptr;
  async::TaskHolder<LoadedData> m_taskHolder;

  void onInputChanged(const std::string& value);
  static arc::Future<LoadedData> fetchLevelsAsync();

protected:
  bool init(float width, float controlHeight, float listHeight, float gap);

public:
  static LeftPanel* create(float width, float controlHeight, float listHeight, float gap);
  void loadLevelList();
  void executeFiltering();
  void filterLevels();
  void setOnSelectedCallback(const std::function<void(std::string)>& onSelected);
  void displayLevelList() const;
};
