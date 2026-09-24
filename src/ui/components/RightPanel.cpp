#include "ui/components/RightPanel.hpp"

#include "handlers/SaveHandler.hpp"

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

  const auto controlRegion = CCScale9Sprite::create("GJ_square05.png");
  controlRegion->setContentSize({width, controlHeight});
  controlRegion->setAnchorPoint({0.5f, 1.f});

  const auto controlMenu = CCMenu::create();
  const auto controlLayout = RowLayout::create();
  controlLayout->setAxisAlignment(AxisAlignment::Even);
  controlMenu->setID("control-menu"_spr);
  controlMenu->setLayout(controlLayout);
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
  controlMenu->addChild(statButton);
  controlMenu->addChild(deleteButton);
  controlMenu->updateLayout();

  controlRegion->addChildAtPosition(controlMenu, Anchor::Center);

  const auto infoRegion = CCScale9Sprite::create("GJ_square05.png");
  infoRegion->setContentSize({width, infoHeight});
  infoRegion->setAnchorPoint({0.5f, 0.f});

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
  infoRegion->addChildAtPosition(m_popupStatusLabel, Anchor::BottomRight, {-width * (6.f / 7.f), -4.f});

  const auto infoMenu = CCMenu::create();
  const auto infoButton = InfoAlertButton::create(
    "General Info Viewer",
    "If info failed to load or show N/A for some, try loading the level again to update.",
    1.f
  );
  infoMenu->addChild(infoButton);
  infoRegion->addChildAtPosition(infoMenu, Anchor::TopRight);
  infoRegion->addChildAtPosition(infoContainer, Anchor::Center);

  addChildAtPosition(controlRegion, Anchor::Top);
  addChildAtPosition(infoRegion, Anchor::Bottom);
  return true;
}

void RightPanel::onPlayButtonClicked(CCObject* sender) {}

void RightPanel::onLinkingButtonClicked(CCObject* sender) {
  const auto toggle = static_cast<CCMenuItemToggler*>(sender);
  const bool wasEnabled = toggle->isToggled(); // state is before

  if (!wasEnabled && m_selectedLevelID.empty()) {
    Notification::create("Select a level", NotificationIcon::Info)->show();
    return;
  }

  const auto str = wasEnabled ? "" : fmt::format("Linking level: {}", m_selectedLevelID);
  m_popupStatusLabel->setString(str.c_str());
}

void RightPanel::onStatButtonClicked(CCObject* sender) {}

void RightPanel::onDeleteButtonClicked(CCObject* sender) {}


void RightPanel::loadLevelInfo(std::string levelID) {
  m_selectedLevelID = levelID;
  const auto [id, name, type] = SaveHandler::getLevelInfo(levelID);

  std::string textContent;
  if (id.empty()) {
    textContent = fmt::format("Failed to read info of\n{}", levelID);
  } else {
    textContent = fmt::format(
      R"(Type: {}
ID: {}
Name: {}
      )",
      type.empty() ? "N/A" : type,
      id.empty() ? "N/A" : id,
      name.empty() ? "N/A" : name
    );
  }

  m_infoLabel->setString(textContent.c_str());
}
