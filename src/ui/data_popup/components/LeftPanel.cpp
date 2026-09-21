#include "ui/data_popup/components/LeftPanel.hpp"

LeftPanel* LeftPanel::create(const CCSize& size) {
  const auto ret = new LeftPanel();
  if (ret->init(size)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool LeftPanel::init(const CCSize& size) {
  if (!CCScale9Sprite::initWithFile("geode.loader/GE_square01.png")) return false;

  this->setContentSize(size);
  const auto scroll = ScrollLayer::create(size - 2.5f);

  scroll->m_contentLayer->setContentWidth(this->getContentWidth());
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

  for (int i = 1; i <= 9; ++i) {
    const auto item = CCNode::create();
    item->setContentSize(
      {
        scroll->m_contentLayer->getContentWidth(),
        30.f
      }
    );

    const auto label = Label::create(fmt::format("test {}", i), "bigFont.fnt");
    item->addChildAtPosition(label, Anchor::Center);

    scroll->m_contentLayer->addChild(item);
  }
  scroll->m_contentLayer->updateLayout();

  this->addChildAtPosition(
      scroll,
      Anchor::Center,
      -scroll->getContentSize() / 2.f
  );
  scroll->scrollToTop();

  return true;
}

void LeftPanel::loadLevelList() {}
