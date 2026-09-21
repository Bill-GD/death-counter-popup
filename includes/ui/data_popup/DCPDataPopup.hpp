#pragma once

#include "components/LeftPanel.hpp"
#include "components/RightPanel.hpp"

using namespace geode::prelude;

class DCPDataPopup : public Popup {
protected:
  bool init() override;

public:
  static DCPDataPopup* create();

private:
  LeftPanel* leftPanel = nullptr;
  RightPanel* rightPanel = nullptr;

  void addContent();
};
