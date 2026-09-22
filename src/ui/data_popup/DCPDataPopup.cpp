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
  m_mainLayer->addChild(this->leftPanel);
  this->leftPanel->setPosition({contentSize.width / 6.f + 5.f, m_mainLayer->getPositionY() - 35.f});

  this->rightPanel = RightPanel::create({contentSize.width / 1.5f - 15.f, contentSize.height - 60.f});
  m_mainLayer->addChild(this->rightPanel);
  this->rightPanel->setPosition({contentSize.width / 1.5f - 5.f, m_mainLayer->getPositionY() - 35.f});

  const auto loadingLayer = CCLayer::create();
  loadingLayer->setContentSize(contentSize);
  loadingLayer->setAnchorPoint({0.5f, 0.5f});
  loadingLayer->setZOrder(3);
  this->m_mainLayer->addChildAtPosition(loadingLayer, Anchor::Center);

  this->loadingCircle = LoadingCircle::create();
  this->loadingCircle->setParentLayer(loadingLayer);
  this->loadingCircle->show();
  this->loadingCircle->setVisible(false);

  m_mainLayer->updateLayout();
}

void DCPDataPopup::onLevelSelected(const std::string& levelID) {
  if (!this->rightPanel) return;
  this->rightPanel->loadLevelInfo(levelID);
}

void DCPDataPopup::load() {
  if (!this->leftPanel || !this->loadingCircle) return;

  this->loadingCircle->setVisible(true);
  this->leftPanel->loadLevelList([this](const std::string& levelID) { onLevelSelected(levelID); });
  this->loadingCircle->setVisible(false);
}
