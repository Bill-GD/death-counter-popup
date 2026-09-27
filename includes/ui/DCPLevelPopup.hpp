#pragma once

#include "types/types.hpp"

using namespace geode::prelude;

class DCPLevelPopup : public Popup {
  ScrollLayer* m_scrollLayer = nullptr;
  std::string m_levelID;
  int m_precisionFilter = 0;
  bool m_fromZeroFilter = true;
  std::vector<KeyedRunData> m_runData;
  std::vector<KeyedRunData> m_filteredRunData;

  void onCheckbox(CCObject* sender);
  void onPrecisionChanged();
  void filterRuns();

protected:
  bool init(const std::string& levelID);

public:
  static DCPLevelPopup* create(const std::string& levelID);
  void load();
  void displayData() const;
};
