#include "ui/DCPLevelPopup.hpp"

#include "ui/components/PrecisionSelector.hpp"

DCPLevelPopup* DCPLevelPopup::create(const GJGameLevel& level) {
  const auto ret = new DCPLevelPopup();
  if (ret->init(level)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool DCPLevelPopup::init(const GJGameLevel& level) {
  if (!Popup::init(440.f, 280.f)) {
    return false;
  }

  setID("level-data-viewer");
  setTitle(fmt::format("Run Counter ({})", level.m_levelName));

  const auto popupSize = m_mainLayer->getScaledContentSize();
  const auto displayWidth = popupSize.width * 0.8f;
  const auto selectorHeight = popupSize.height * 0.15f;

  const auto precisionSelectorArea = CCNode::create();
  precisionSelectorArea->setContentHeight(selectorHeight);
  precisionSelectorArea->setAnchorPoint({0.5f, 1.f});

  const auto label = CCLabelBMFont::create("Precision", "bigFont.fnt");
  label->setScale(0.5f);
  label->setAnchorPoint({1.f, 0.5f});
  precisionSelectorArea->addChildAtPosition(label, Anchor::Left, {-15.f, 0.f});

  const auto precisionSelector = PrecisionSelector::create(
    {displayWidth * 0.2f, selectorHeight},
    [this](std::string value) {
    }
  );
  precisionSelector->setAnchorPoint({0.f, 0.5f});
  precisionSelectorArea->addChildAtPosition(precisionSelector, Anchor::Right, {15.f, 0.f});

  const auto dataArea = CCScale9Sprite::create("GJ_square05.png");
  dataArea->setContentSize({displayWidth, popupSize.height * 0.65f});
  dataArea->setAnchorPoint({0.5f, 0.f});

  m_mainLayer->addChildAtPosition(precisionSelectorArea, Anchor::Top, {0.f, -35.f});
  m_mainLayer->addChildAtPosition(dataArea, Anchor::Bottom, {0.f, 20.f});

  return true;
}

void DCPLevelPopup::load() {}
