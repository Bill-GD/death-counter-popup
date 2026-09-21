#include "ui/data_popup/DCPDataPopup.hpp"

#include "ui/data_popup/components/LeftPanel.hpp"
#include "ui/data_popup/components/RightPanel.hpp"

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
  if (!Popup::init(440.f, 280.f)) {
    return false;
  }

  this->setID("dcp-data-viewer");
  this->setTitle("Run Counter Data Viewer");

  addContent();

  return true;
}

void DCPDataPopup::addContent() {
  this->setAnchorPoint({0.5f, 0.5f});

  const auto contentSize = m_mainLayer->getScaledContentSize();
  this->leftPanel = LeftPanel::create({contentSize.width / 3.f - 15.f, contentSize.height - 60.f});
  this->rightPanel = RightPanel::create();

  m_mainLayer->addChild(this->leftPanel);
  m_mainLayer->addChild(this->rightPanel);

  // this->leftPanel->setContentSize({contentSize.width / 3.f - 15.f, contentSize.height - 60.f});
  this->rightPanel->setContentSize({contentSize.width / 1.5f - 15.f, contentSize.height - 60.f});

  this->leftPanel->setPosition({contentSize.width / 6.f + 5.f, m_mainLayer->getPositionY() - 35.f});
  this->rightPanel->setPosition({contentSize.width / 1.5f - 5.f, m_mainLayer->getPositionY() - 35.f});

  m_mainLayer->updateLayout();
}
