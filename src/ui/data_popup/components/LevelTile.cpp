#include "ui/data_popup/components/LevelTile.hpp"

#include "handlers/SaveHandler.hpp"

LevelTile* LevelTile::create(
  const std::string& levelID, const CCSize& size, const std::function<void(std::string)>& onSelected
) {
  const auto ret = new LevelTile();
  if (ret->init(levelID, size, onSelected)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool LevelTile::init(std::string levelID, const CCSize& size, std::function<void(std::string)> onSelected) {
  if (!CCNode::init()) return false;

  this->onLevelSelected = std::move(onSelected);

  this->setContentSize(size);

  const auto textClip = CCClippingNode::create();
  textClip->setContentWidth(this->getContentWidth() * 0.7f);
  textClip->setAnchorPoint({0.f, 0.5f});

  const auto stencil = CCScale9Sprite::create("geode.loader/GE_square01.png");
  stencil->setContentSize(textClip->getContentSize());
  stencil->setAnchorPoint({0.f, 0.5f});
  stencil->setPosition({0.f, textClip->getContentHeight() / 2.f});
  textClip->setStencil(stencil);

  const auto label = Label::create(fmt::format("{}", levelID), "bigFont.fnt");
  label->setScale(0.3f);
  label->setAnchorPoint({0.f, 0.5f});
  label->setAlignment(Label::Alignment::Left);
  textClip->addChildAtPosition(label, Anchor::Left);
  this->addChildAtPosition(textClip, Anchor::Left, {10.f, 0});

  const auto menu = CCMenu::create();
  menu->setContentSize({this->getContentWidth() * 0.25f, this->getContentHeight()});
  menu->setAnchorPoint({1.f, 0.5f});

  const auto arrow = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
    this,
    menu_selector(LevelTile::onTileClicked)
  );
  arrow->setRotation(180.f);
  arrow->setAnchorPoint({0.5f, 0.5f});
  menu->setScale(0.5f);
  menu->addChildAtPosition(arrow, Anchor::Center);
  this->addChildAtPosition(menu, Anchor::Right, {-16.f, 0});

  arrow->setUserObject(CCString::create(levelID));

  return true;
}

void LevelTile::onTileClicked(CCObject* sender) const {
  const auto tile = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
  const auto levelID = typeinfo_cast<CCString*>(tile->getUserObject())->getCString();
  if (onLevelSelected) onLevelSelected(levelID);
}
