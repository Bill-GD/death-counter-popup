#include "hooks/DCPEditLevelLayer.hpp"
#include "Geode/Geode.hpp"
#include "handlers/SaveHandler.hpp"
#include "handlers/settings/Settings.hpp"
#include "ui/components/DCPDataViewerButton.hpp"
#include "utils/LevelUtils.hpp"

bool DCPEditLevelLayer::init(GJGameLevel* level) {
  if (!EditLevelLayer::init(level)) {
    return false;
  }

  SaveHandler::setLevel(level);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  if (Settings::popupButtonShown()) {
    const auto infoButtonMenu = getChildByID("info-button-menu");

    const auto dtButton = infoButtonMenu->getChildByID("dt-skull-button");
    const auto infoButton = infoButtonMenu->getChildByID("info-button");
    const auto settingsButton = infoButtonMenu->getChildByID("settings-button");

    const auto dcpButton = DCPDataViewerButton::create(0.75f);
    infoButtonMenu->addChild(dcpButton);

    if (LevelUtils::isModLoaded("elohmrow.death_tracker") && dtButton) {
      dcpButton->setPosition({dtButton->getPositionX(), settingsButton->getPositionY()});
    } else {
      dcpButton->setPosition(
        {
          infoButton->getPositionX() + dcpButton->getScaledContentWidth(),
          infoButton->getPositionY()
        }
      );
    }
    infoButtonMenu->updateLayout();
  }

  return true;
}
