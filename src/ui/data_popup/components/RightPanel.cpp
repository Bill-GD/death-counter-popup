#include "ui/data_popup/components/RightPanel.hpp"

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

  const auto testLabel = Label::create("test", "bigFont.fnt");
  testLabel->setPosition(this->getPosition());
  this->addChildAtPosition(testLabel, Anchor::Center);

  return true;
}

void RightPanel::loadLevelInfo(std::string levelID) {
  const auto testLabel = Label::create(fmt::format("{}", levelID), "bigFont.fnt");
  testLabel->setPosition(this->getPosition());
  this->addChildAtPosition(testLabel, Anchor::Center);
}
