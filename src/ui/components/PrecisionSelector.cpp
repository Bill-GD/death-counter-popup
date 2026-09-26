#include "ui/components/PrecisionSelector.hpp"

#include "utils/Constants.hpp"

PrecisionSelector* PrecisionSelector::create(
  const CCSize& size,
  const std::function<void(std::string const&)>& callback
) {
  const auto ret = new PrecisionSelector();
  if (ret->init(size, callback)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool PrecisionSelector::init(
  const CCSize& size,
  const std::function<void(std::string const&)>& callback
) {
  if (!CCNode::init()) {
    return false;
  }

  m_values.reserve(Constants::MAX_PRECISION + 1);
  for (int i = 0; i <= Constants::MAX_PRECISION; ++i) {
    m_values.push_back(std::to_string(i));
  }
  m_callback = std::move(callback);

  setContentSize(size);

  const auto leftSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
  leftSprite->setScaleY(0.35f);
  leftSprite->setScaleX(-0.35f);
  const auto leftButton = CCMenuItemSpriteExtra::create(
    leftSprite,
    this,
    menu_selector(PrecisionSelector::onLeft)
  );

  const auto rightSprite = CCSprite::createWithSpriteFrameName("navArrowBtn_001.png");
  rightSprite->setScale(0.35);
  const auto rightButton = CCMenuItemSpriteExtra::create(
    rightSprite,
    this,
    menu_selector(PrecisionSelector::onRight)
  );

  m_label = CCLabelBMFont::create("", "bigFont.fnt");
  m_label->setScale(0.55f);
  m_label->setAnchorPoint({0.5f, 0.5f});

  const auto menu = CCMenu::create();
  menu->setContentSize(getContentSize());

  leftButton->setAnchorPoint({0.f, 0.5f});
  rightButton->setAnchorPoint({1.f, 0.5f});
  menu->addChildAtPosition(leftButton, Anchor::Left);
  menu->addChildAtPosition(rightButton, Anchor::Right);
  menu->addChildAtPosition(m_label, Anchor::Center);

  leftButton->setAnchorPoint({0.5f, 0.5f});
  rightButton->setAnchorPoint({0.5f, 0.5f});

  addChildAtPosition(menu, Anchor::Center);
  updateLabel();

  return true;
}

void PrecisionSelector::onLeft(CCObject*) {
  if (m_index == 0) {
    m_index = m_values.size() - 1;
  } else {
    m_index--;
  }

  updateLabel();
}

void PrecisionSelector::onRight(CCObject*) {
  m_index = (m_index + 1) % m_values.size();
  updateLabel();
}

void PrecisionSelector::updateLabel() {
  const auto& value = m_values.at(m_index);
  m_label->setString(value.c_str());
  m_callback(value);
}
