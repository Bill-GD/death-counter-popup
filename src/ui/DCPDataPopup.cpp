#include "ui/DCPDataPopup.hpp"

DCPDataPopup* DCPDataPopup::create() {
  const auto ret = new DCPDataPopup();
  if (ret->init()) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool DCPDataPopup::init() {
  if (!Popup::init(440.f, 280.f, "geode.loader/GE_square01.png")) {
    return false;
  }

  this->setID("data-viewer");
  this->setTitle("Run Counter Data Viewer");

  m_mainLayer->addChild(getContent());

  m_mainLayer->updateLayout();
  return true;
}

CCNode* DCPDataPopup::getContent() {
  const auto content = ScrollLayer::create(
    {m_size.width, m_size.height - 40}
  );
  content->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout());

  content->setAnchorPoint({0.5f, 0.5f});

  content->updateLayout();
  return content;
}
