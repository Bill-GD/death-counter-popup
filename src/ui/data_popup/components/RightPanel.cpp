#include "ui/data_popup/components/RightPanel.hpp"

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
  controlMenu->setLayout(controlLayout);
  controlMenu->setContentSize({width, controlHeight});

  const auto buttonHeight = controlHeight - 10.f;

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

  const auto infoRegion = CCScale9Sprite::create("GJ_square05.png");
  infoRegion->setContentSize({width, infoHeight});
  infoRegion->setAnchorPoint({0.5f, 0.f});

  const auto infoContainer = CCNode::create();
  infoContainer->setContentSize({width * 0.9f, infoHeight * 0.85f});
  infoContainer->setAnchorPoint({0.5f, 0.5f});

  infoLabel = Label::create("", "bigFont.fnt");
  infoLabel->setScale(0.5f);
  infoLabel->setAlignment(Label::Alignment::Left);
  infoLabel->setAnchorPoint({0.f, 1.f});
  infoContainer->addChildAtPosition(infoLabel, Anchor::Top);

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

void RightPanel::loadLevelInfo(std::string levelID) {
  selectedLevelID = levelID;
  const auto [id, name, type] = SaveHandler::getLevelInfo(levelID);

  std::string textContent;
  if (id.empty()) {
    textContent = fmt::format("Failed to read\ninfo of {}", levelID);
  } else {
    textContent = fmt::format(
      R"(
Type: {}
ID: {}
Name: {}
      )",
      type.empty() ? "N/A" : type,
      id.empty() ? "N/A" : id,
      name.empty() ? "N/A" : name
    );
  }

  infoLabel->setString(textContent.c_str());
}
