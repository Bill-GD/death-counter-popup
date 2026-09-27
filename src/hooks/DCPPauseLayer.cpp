#include "hooks/DCPPauseLayer.hpp"

#include "handlers/settings/Settings.hpp"
#include "ui/components/DCPDataViewerButton.hpp"

void DCPPauseLayer::customSetup() {
  PauseLayer::customSetup();

  if (Settings::popupButtonShown()) {
    const auto sideMenu = getChildByID("left-button-menu");
    sideMenu->addChild(DCPDataViewerButton::create(0.75f));
    sideMenu->updateLayout();
  }
}
