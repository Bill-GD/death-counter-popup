#include "ui/components/DCPDataViewerButton.hpp"

#include "ui/DCPDataPopup.hpp"

DCPDataViewerButton* DCPDataViewerButton::create(const float scale) {
  const auto ret = new DCPDataViewerButton();
  if (ret->init(scale)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool DCPDataViewerButton::init(const float scale) {
  const auto baseSprite = CCSprite::createWithSpriteFrameName("GJ_plainBtn_001.png");
  if (!baseSprite) return false;
  baseSprite->setScale(scale);

  const auto skull = CCSprite::createWithSpriteFrameName("miniSkull_001.png");
  if (!skull) return false;
  skull->setScale(0.6f);

  const auto text = Label::create("Popup", "bigFont.fnt");
  if (!text) return false;
  text->setScale(0.3f);

  baseSprite->addChildAtPosition(skull, Anchor::Center, {0.f, 5.f});
  baseSprite->addChildAtPosition(text, Anchor::Center, {0.f, -7.f});

  if (!CCMenuItemSpriteExtra::init(baseSprite, nullptr, this, menu_selector(DCPDataViewerButton::onClick))) {
    return false;
  }

  setContentSize(baseSprite->getScaledContentSize());

  return true;
}

void DCPDataViewerButton::onClick(CCObject*) {
  const auto popup = DCPDataPopup::create();
  if (!popup) return;
  popup->show();
  popup->load();
}
