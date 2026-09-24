#include "ui/components/LevelTile.hpp"

#include "handlers/SaveHandler.hpp"

LevelTile* LevelTile::create(
  const LevelTileInfo& tileInfo,
  const CCSize& size,
  const std::function<void(std::string)>& onSelected
) {
  const auto ret = new LevelTile();
  if (ret->init(tileInfo, size, onSelected)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool LevelTile::init(LevelTileInfo tileInfo, const CCSize& size, std::function<void(std::string)> onSelected) {
  if (!CCNode::init()) return false;

  m_onLevelSelected = std::move(onSelected);

  setContentSize(size);

  const auto textClip = CCClippingNode::create();
  textClip->setContentWidth(getContentWidth() * 0.7f);
  textClip->setAnchorPoint({0.f, 0.5f});

  const auto stencil = CCScale9Sprite::create("geode.loader/GE_square01.png");
  stencil->setContentSize(textClip->getContentSize());
  stencil->setAnchorPoint({0.f, 0.5f});
  stencil->setPosition({0.f, textClip->getContentHeight() / 2.f});
  textClip->setStencil(stencil);

  const auto nameLabel = Label::create(
    fmt::format("{}", tileInfo.name.empty() ? "N/A" : tileInfo.name),
    "bigFont.fnt"
  );
  nameLabel->setScale(0.35f);
  nameLabel->setAnchorPoint({0.f, 0.5f});
  nameLabel->setAlignment(Label::Alignment::Left);

  const auto idLabel = Label::create(fmt::format("{}", tileInfo.id), "bigFont.fnt");
  idLabel->setScale(0.3f);
  idLabel->setColor(ccColor3B{200, 200, 200});
  idLabel->setAnchorPoint({0.f, 0.5f});
  idLabel->setAlignment(Label::Alignment::Left);

  textClip->addChildAtPosition(nameLabel, Anchor::Left, {0, 5.f});
  textClip->addChildAtPosition(idLabel, Anchor::Left, {0, -5.f});
  addChildAtPosition(textClip, Anchor::Left, {10.f, 0});

  const auto menu = CCMenu::create();
  menu->setContentSize({getContentWidth() * 0.25f, getContentHeight()});
  menu->setAnchorPoint({1.f, 0.5f});

  const auto arrow = CCMenuItemSpriteExtra::create(
    CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png"),
    this,
    menu_selector(LevelTile::onTileClicked)
  );
  arrow->setRotation(180.f);
  arrow->setAnchorPoint({0.5f, 0.5f});
  arrow->setUserObject(CCString::create(tileInfo.id));

  menu->setScale(0.55f);
  menu->addChildAtPosition(arrow, Anchor::Center);
  addChildAtPosition(menu, Anchor::Right, {-12.f, 0});

  return true;
}

void LevelTile::onTileClicked(CCObject* sender) const {
  const auto tile = typeinfo_cast<CCMenuItemSpriteExtra*>(sender);
  const auto levelID = typeinfo_cast<CCString*>(tile->getUserObject())->getCString();
  if (m_onLevelSelected) m_onLevelSelected(levelID);
}
