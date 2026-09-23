#include "ui/data_popup/components/RightPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "utils/FileUtils.hpp"

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
  controlMenu->setLayout(controlLayout);
  controlMenu->setContentSize({width, controlHeight});

  const auto buttonHeight = controlHeight - 5.f;

  const auto playSprite = CCSprite::createWithSpriteFrameName("GJ_playBtn2_001.png");
  playSprite->setScale(buttonHeight / playSprite->getContentHeight());
  const auto playButton = CCMenuItemSpriteExtra::create(
    playSprite,
    this,
    nullptr
  );
  playButton->setContentSize({buttonHeight, buttonHeight});

  // gj_linkBtnOff_001.png
  // gj_linkBtn_001.png
  const auto linkSprite = CCSprite::createWithSpriteFrameName("gj_linkBtn_001.png");
  linkSprite->setScale(buttonHeight / linkSprite->getContentHeight());
  const auto linkButton = CCMenuItemSpriteExtra::create(
    linkSprite,
    this,
    nullptr
  );
  linkButton->setContentSize({buttonHeight, buttonHeight});

  const auto statSprite = CCSprite::createWithSpriteFrameName("GJ_statsBtn_001.png");
  statSprite->setScale(buttonHeight / statSprite->getContentHeight());
  const auto statButton = CCMenuItemSpriteExtra::create(
    statSprite,
    this,
    nullptr
  );
  statButton->setContentSize({buttonHeight, buttonHeight});

  const auto deleteSprite = CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png");
  deleteSprite->setScale(buttonHeight / deleteSprite->getContentHeight());
  const auto deleteButton = CCMenuItemSpriteExtra::create(
    deleteSprite,
    this,
    nullptr
  );
  deleteButton->setContentSize({buttonHeight, buttonHeight});

  controlMenu->addChild(playButton);
  controlMenu->addChild(linkButton);
  controlMenu->addChild(statButton);
  controlMenu->addChild(deleteButton);
  controlMenu->updateLayout();
  controlRegion->addChildAtPosition(controlMenu, Anchor::Center);

  infoRegion = CCScale9Sprite::create("GJ_square05.png");
  infoRegion->setContentSize({width, infoHeight});
  infoRegion->setAnchorPoint({0.5f, 0.f});

  const auto infoMenu = CCMenu::create();
  const auto infoButton = InfoAlertButton::create(
    "General Info Viewer",
    "If info failed to load or show N/A for some, try loading the level again to update.",
    1.f
  );
  infoMenu->addChild(infoButton);
  infoRegion->addChildAtPosition(infoMenu, Anchor::TopRight);

  addChildAtPosition(controlRegion, Anchor::Top);
  addChildAtPosition(infoRegion, Anchor::Bottom);
  return true;
}

void RightPanel::loadLevelInfo(std::string levelID) {
  const auto infoPath = SaveHandler::PATH / levelID / "info";
  const auto [success, value] = FileUtils::tryRead(infoPath);

  if (infoTextContainer) {
    infoTextContainer->removeFromParent();
    infoTextContainer = nullptr;
  }
  if (messageLabel) {
    messageLabel->removeFromParent();
    messageLabel = nullptr;
  }

  if (!success) {
    messageLabel = Label::create(fmt::format("Failed to read\ninfo of {}", levelID), "bigFont.fnt");
    messageLabel->setScale(0.5f);
    messageLabel->setAnchorPoint({0.5f, 1.f});
    infoRegion->addChildAtPosition(messageLabel, Anchor::Top);
    return;
  }

  const auto [id, name, type] = Utils::tryParse<LevelInfo>(value);

  const auto panelSize = infoRegion->getScaledContentSize();
  const auto container = CCNode::create();
  container->setContentSize(panelSize);
  container->setAnchorPoint({0.5f, 0.5f});

  const auto scroll = ScrollLayer::create(panelSize - 2.5f);
  scroll->m_contentLayer->setContentSize({panelSize.width, panelSize.height - 4.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  container->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scroll->getContentSize() / 2.f
  );

  const auto typeLabel = Label::create(fmt::format("Type: {}", type.empty() ? "N/A" : type), "bigFont.fnt");
  typeLabel->setScale(0.5f);
  typeLabel->setAlignment(Label::Alignment::Left);
  scroll->m_contentLayer->addChildAtPosition(typeLabel, Anchor::Left);

  const auto idLabel = Label::create(fmt::format("ID: {}", id.empty() ? "N/A" : id), "bigFont.fnt");
  idLabel->setScale(0.5f);
  idLabel->setAlignment(Label::Alignment::Left);
  scroll->m_contentLayer->addChildAtPosition(idLabel, Anchor::Left);

  const auto nameLabel = Label::create(fmt::format("Name: {}", name.empty() ? "N/A" : name), "bigFont.fnt");
  nameLabel->setScale(0.5f);
  nameLabel->setAlignment(Label::Alignment::Left);
  scroll->m_contentLayer->addChildAtPosition(nameLabel, Anchor::Left);

  scroll->m_contentLayer->updateLayout();
  scroll->scrollToTop();

  infoRegion->addChildAtPosition(container, Anchor::Center);
  infoTextContainer = container;
}
