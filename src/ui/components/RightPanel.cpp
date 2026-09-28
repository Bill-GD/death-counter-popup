#include "ui/components/RightPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "ui/DCPDataPopup.hpp"
#include "ui/DCPLevelPopup.hpp"
#include "utils/LevelUtils.hpp"

RightPanel* RightPanel::create(const float width, const float controlHeight, const float infoHeight, const float gap) {
  const auto ret = new RightPanel();
  if (ret->init(width, controlHeight, infoHeight, gap)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool RightPanel::init(float width, float controlHeight, float infoHeight, float gap) {
  if (!CCNode::init()) return false;
  setContentSize({width, infoHeight + gap + controlHeight});

  const auto controlArea = CCScale9Sprite::create("GJ_square05.png");
  controlArea->setContentSize({width, controlHeight});
  controlArea->setAnchorPoint({0.5f, 1.f});

  const auto controlMenu = CCMenu::create();
  controlMenu->setID("level-control-menu");
  controlMenu->setLayout(RowLayout::create()->setAxisAlignment(AxisAlignment::Even));
  controlMenu->setContentSize({width, controlHeight});

  const auto buttonHeight = controlHeight - 10.f;

  const auto playSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
  playSprite->setScale(buttonHeight / playSprite->getContentHeight());
  const auto playButton = CCMenuItemSpriteExtra::create(
    playSprite,
    this,
    menu_selector(RightPanel::onPlayButtonClicked)
  );
  playButton->setContentSize({buttonHeight, buttonHeight});

  const auto linkSprite = CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png");
  const auto unlinkSprite = CCSprite::createWithSpriteFrameName("gj_linkBtnOff_001.png");
  linkSprite->setScale(buttonHeight / linkSprite->getContentHeight());
  unlinkSprite->setScale(buttonHeight / unlinkSprite->getContentHeight());
  m_linkingButton = CCMenuItemToggler::create(
    linkSprite,
    unlinkSprite,
    this,
    menu_selector(RightPanel::onLinkingButtonClicked)
  );
  m_linkingButton->setContentSize({buttonHeight, buttonHeight});
  m_linkingButton->toggle(false);

  const auto loadCurrentText = Label::create("ID", "bigFont.fnt");
  loadCurrentText->setScale(0.7f);
  const auto loadCurrentSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
  loadCurrentSprite->setScale(buttonHeight / loadCurrentSprite->getContentHeight());
  loadCurrentSprite->addChildAtPosition(loadCurrentText, Anchor::Center);
  const auto loadCurrentButton = CCMenuItemSpriteExtra::create(
    loadCurrentSprite,
    this,
    menu_selector(RightPanel::onLoadCurrentButtonClicked)
  );
  loadCurrentButton->setContentSize({buttonHeight, buttonHeight});

  const auto statSprite = CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png");
  statSprite->setScale(buttonHeight / statSprite->getContentHeight());
  const auto statButton = CCMenuItemSpriteExtra::create(
    statSprite,
    this,
    menu_selector(RightPanel::onStatButtonClicked)
  );
  statButton->setContentSize({buttonHeight, buttonHeight});

  const auto deleteSprite = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
  deleteSprite->setScale(buttonHeight / deleteSprite->getContentHeight());
  const auto deleteButton = CCMenuItemSpriteExtra::create(
    deleteSprite,
    this,
    menu_selector(RightPanel::onDeleteButtonClicked)
  );
  deleteButton->setContentSize({buttonHeight, buttonHeight});

  controlMenu->addChild(playButton);
  controlMenu->addChild(m_linkingButton);
  controlMenu->addChild(loadCurrentButton);
  controlMenu->addChild(statButton);
  controlMenu->addChild(deleteButton);
  controlMenu->updateLayout();

  controlArea->addChildAtPosition(controlMenu, Anchor::Center);

  const auto infoArea = CCScale9Sprite::create("GJ_square05.png");
  infoArea->setContentSize({width, infoHeight});
  infoArea->setAnchorPoint({0.5f, 0.f});

  const auto infoContainer = CCNode::create();
  infoContainer->setContentSize({width * 0.9f, infoHeight * 0.85f});
  infoContainer->setAnchorPoint({0.5f, 0.5f});

  m_infoLabel = Label::create("", "bigFont.fnt");
  m_infoLabel->setID("level-info-label");
  m_infoLabel->setScale(0.4f);
  m_infoLabel->setAlignment(Label::Alignment::Left);
  m_infoLabel->setAnchorPoint({0.5f, 1.f});

  infoContainer->addChildAtPosition(m_infoLabel, Anchor::Top);

  m_popupStatusLabel = Label::create("", "bigFont.fnt");
  m_popupStatusLabel->setID("popup-status-label");
  m_popupStatusLabel->setScale(0.5f);
  m_popupStatusLabel->setAnchorPoint({0.5f, 1.f});

  const auto infoMenu = CCMenu::create();
  const auto infoButton = InfoAlertButton::create(
    "General Info Viewer",
    R"(If info failed to load or show N/A for some, try loading the level to update.
If the level doesn't fully load, try playing it once (only playing will load main levels).
Fully deleted levels can't be loaded, I can't do anything about that.)",
    1.f
  );
  infoMenu->addChild(infoButton);

  infoArea->addChildAtPosition(infoMenu, Anchor::TopRight);
  infoArea->addChildAtPosition(infoContainer, Anchor::Center);
  infoArea->addChildAtPosition(m_popupStatusLabel, Anchor::BottomRight, {-width * (6.f / 7.f), -4.f});

  addChildAtPosition(controlArea, Anchor::Top);
  addChildAtPosition(infoArea, Anchor::Bottom);
  return true;
}

void RightPanel::onPlayButtonClicked(CCObject*) {
  if (m_selectedLevelID.empty()) return;

  const auto [id, levelType, gameLevel] = LevelUtils::getLevel(m_selectedLevelID);
  if (!gameLevel) {
    Notification::create("Could not get level, aborted", NotificationIcon::Info)->show();
    return;
  }

  CCScene* scene;
  switch (levelType) {
    case GJLevelType::Main: {
      scene = LevelSelectLayer::scene(std::max(id - 1, 0));
      Notification::create("Remember: play once to update info.")->show();
      break;
    }
    case GJLevelType::Editor: {
      scene = EditLevelLayer::scene(gameLevel);
      break;
    }
    default: {
      scene = LevelInfoLayer::scene(gameLevel, false);
      break;
    }
  }

  DCPDataPopup::closePopup();
  const auto ccDirector = CCDirector::sharedDirector();
  if (ccDirector->getRunningScene()) ccDirector->replaceScene(scene);
  else ccDirector->pushScene(scene);
}

void RightPanel::onLinkingButtonClicked(CCObject* sender) {
  const auto toggle = static_cast<CCMenuItemToggler*>(sender);
  const bool wasEnabled = toggle->isToggled(); // state is before

  if (!wasEnabled && m_selectedLevelID.empty()) return;

  const auto str = wasEnabled ? "" : fmt::format("Linking level: {}", m_selectedLevelID);
  m_popupStatusLabel->setString(str.c_str());
}

void RightPanel::onLoadCurrentButtonClicked(CCObject*) {
  if (!SaveHandler::isLevelSet()) return;
  loadLevelInfo(SaveHandler::getCurrentLevelID());
}

void RightPanel::onStatButtonClicked(CCObject*) {
  if (m_selectedLevelID.empty()) return;

  const auto popup = DCPLevelPopup::create(m_selectedLevelID);
  popup->show();
  popup->load();
}

void RightPanel::onDeleteButtonClicked(CCObject*) {
  if (m_selectedLevelID.empty()) return;

  createQuickPopup(
    "Confirm Delete",
    fmt::format("Are you sure you want to delete\nsaved data of <cy>{}</c>?", m_selectedLevelID),
    "Cancel",
    "Delete",
    [this, selectedLevelID = m_selectedLevelID](FLAlertLayer*, const bool confirmed) {
      // confirmed is true when btn2 clicked
      if (!confirmed) return;
      if (SaveHandler::deleteSavedData(selectedLevelID)) {
        Notification::create(
          fmt::format("Deleted data of {}. Remember to reload.", selectedLevelID),
          NotificationIcon::Success
        )->show();
      } else {
        Notification::create(
          fmt::format("Failed to delete data of {}", selectedLevelID),
          NotificationIcon::Error
        )->show();
      }
      unloadLevelInfo();
    },
    true,
    true
  );
}

void RightPanel::loadLevelInfo(std::string levelID) {
  m_selectedLevelID = levelID;
  const auto [numID, levelType, _1, _2] = LevelUtils::parseLevelID(m_selectedLevelID);
  const auto [savedID, name, link] = SaveHandler::getLevelInfoFromFile(m_selectedLevelID);

  std::string textContent;
  if (savedID.empty()) {
    textContent = fmt::format("Failed to read info of\n{}", levelID);
  } else {
    textContent = fmt::format(
      R"(Type: {}
ID: {}
Name: {}
Link: {}
      )",
      LevelUtils::levelTypeToString(levelType),
      savedID.empty() ? "N/A" : savedID,
      name.empty() ? "N/A" : name,
      link.empty() ? "N/A" : link
    );
  }

  m_infoLabel->setString(textContent.c_str());
}

void RightPanel::unloadLevelInfo() {
  m_selectedLevelID = "";
  m_infoLabel->setString("");
}
