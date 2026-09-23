#pragma once

#include "components/LeftPanel.hpp"
#include "components/RightPanel.hpp"

using namespace geode::prelude;

class DCPDataPopup : public Popup {
protected:
  bool init() override;

public:
  static DCPDataPopup* create();
  void load() const;

private:
  LeftPanel* m_leftPanel = nullptr;
  RightPanel* m_rightPanel = nullptr;
  LoadingCircle* m_loadingCircle = nullptr;

  void addContent();
  void onLevelSelected(const std::string& levelID) const;
};
