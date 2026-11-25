#include "ui/components/PrecisionSelector.hpp"

#include "utils/Constants.hpp"

PrecisionSelector* PrecisionSelector::create(
  const CCSize& size,
  const std::function<void(int const&)>& callback
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
  const std::function<void(int const&)>& callback
) {
  if (!CCNode::init()) {
    return false;
  }

  m_values.reserve(Constants::MAX_PRECISION + 1);
  for (int i = 0; i <= Constants::MAX_PRECISION; ++i) {
    m_values.push_back(i);
  }
  m_callback = std::move(callback);

  setContentSize(size);

  const auto leftSprite = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
  leftSprite->setScale(0.75f);
  const auto leftButton = CCMenuItemSpriteExtra::create(
    leftSprite,
    this,
    menu_selector(PrecisionSelector::onLeft)
  );

  const auto rightSprite = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
  rightSprite->setScale(0.75f);
  const auto rightButton = CCMenuItemSpriteExtra::create(
    rightSprite,
    this,
    menu_selector(PrecisionSelector::onRight)
  );

  m_label = CCLabelBMFont::create("", "bigFont.fnt");
  m_label->setScale(0.6f);
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

void PrecisionSelector::updateLabel() const {
  const auto& value = m_values.at(m_index);
  m_label->setString(std::to_string(value).c_str());
  m_callback(value);
}
