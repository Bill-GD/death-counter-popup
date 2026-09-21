#include "ui/data_popup/components/RightPanel.hpp"

RightPanel* RightPanel::create() {
  const auto ret = new RightPanel();
  if (ret->init()) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool RightPanel::init() {
  if (!CCScale9Sprite::initWithFile("geode.loader/GE_square01.png")) return false;

  this->setAnchorPoint({0.5f, 0.5f});
  const auto testLabel = Label::create("test", "bigFont.fnt");
  testLabel->setPosition(this->getPosition());
  this->addChild(testLabel);

  return true;
}

void RightPanel::loadLevel(std::string levelID) {}
