#pragma once

#include "components/LeftPanel.hpp"
#include "components/RightPanel.hpp"

using namespace geode::prelude;

class DCPDataPopup : public Popup {
  LeftPanel* m_leftPanel = nullptr;
  RightPanel* m_rightPanel = nullptr;

  void onLevelSelected(const std::string& levelID) const;
  void onReload(CCObject*);

protected:
  bool init() override;

public:
  static DCPDataPopup* create();
  static void closePopup();
  void load() const;
};
