#include "hooks/DCPLevelInfoLayer.hpp"

#include "Geode/Geode.hpp"
#include "handlers/SaveHandler.hpp"
#include "handlers/settings/Settings.hpp"
#include "ui/components/DCPDataViewerButton.hpp"
#include "utils/LevelUtils.hpp"

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  SaveHandler::setLevel(level);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  if (Settings::popupButtonShown()) {
    const auto otherMenu = getChildByID("other-menu");
    const auto settingsMenu = getChildByID("settings-menu");
    if (!otherMenu || !settingsMenu) return true;

    const auto dtButton = otherMenu->getChildByID("dt-skull-button");
    const auto favButton = otherMenu->getChildByID("favorite-button");
    const auto settingsButton = settingsMenu->getChildByID("settings-button");

    if (!favButton || !settingsButton) return true;

    const auto dcpButton = DCPDataViewerButton::create(0.75f);
    otherMenu->addChild(dcpButton);

    if (LevelUtils::isModLoaded("elohmrow.death_tracker") && dtButton) {
      dcpButton->setPosition(
        {
          dtButton->getPositionX(),
          dtButton->getPositionY() + dtButton->getScaledContentHeight() + 5.f
        }
      );
    } else if (favButton->isVisible()) {
      dcpButton->setPosition({favButton->getPositionX(), settingsButton->getPositionY()});
    } else {
      dcpButton->setPosition(favButton->getPosition());
    }
    otherMenu->updateLayout();
  }

  return true;
}
