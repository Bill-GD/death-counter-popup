#include "ui/data_popup/components/RightPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "utils/FileUtils.hpp"

RightPanel* RightPanel::create(const CCSize& size) {
  const auto ret = new RightPanel();
  if (ret->init(size)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool RightPanel::init(const CCSize& size) {
  if (!CCScale9Sprite::initWithFile("geode.loader/GE_square01.png")) return false;

  this->setContentSize(size);

  const auto infoMenu = CCMenu::create();
  const auto infoButton = InfoAlertButton::create(
    "Help",
    "If info failed to load or show N/A for some, try loading the level again to update.",
    1.f
  );
  infoMenu->addChild(infoButton);
  addChildAtPosition(infoMenu, Anchor::TopRight);

  return true;
}

void RightPanel::loadLevelInfo(std::string levelID) {
  const auto infoPath = SaveHandler::PATH / levelID / "info";
  const auto [success, value] = FileUtils::tryRead(infoPath);

  if (infoContainer) {
    infoContainer->removeFromParent();
    infoContainer = nullptr;
  }
  if (messageLabel) {
    messageLabel->removeFromParent();
    messageLabel = nullptr;
  }

  if (!success) {
    messageLabel = Label::create(fmt::format("Failed to read\ninfo of {}", levelID), "bigFont.fnt");
    messageLabel->setScale(0.5f);
    addChildAtPosition(messageLabel, Anchor::Center);
    return;
  }

  const auto [id, name, type] = Utils::tryParse<LevelInfo>(value);

  const auto panelSize = getScaledContentSize();
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

  addChildAtPosition(
    container,
    Anchor::Center,
    {0, -15.f}
  );
  infoContainer = container;
}
