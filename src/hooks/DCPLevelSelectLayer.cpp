#include "hooks/DCPLevelSelectLayer.hpp"

#include "Geode/Geode.hpp"
#include "handlers/settings/Settings.hpp"
#include "ui/components/DCPDataViewerButton.hpp"

bool DCPLevelSelectLayer::init(int page) {
  if (!LevelSelectLayer::init(page)) {
    return false;
  }

  if (Settings::popupButtonShown()) {
    const auto infoMenu = getChildByID("info-menu");
    if (!infoMenu) return true;

    const auto infoButton = infoMenu->getChildByID("info-button");
    if (!infoButton) return true;

    const auto dcpButton = DCPDataViewerButton::create(0.525f);
    infoMenu->addChild(dcpButton);

    if (infoButton->isVisible()) {
      dcpButton->setPosition(
        {
          infoButton->getPositionX() - infoButton->getScaledContentHeight() - 5.f,
          infoButton->getPositionY(),
        }
      );
    } else {
      dcpButton->setPosition(infoButton->getPosition());
    }
    infoMenu->updateLayout();
  }

  return true;
}
