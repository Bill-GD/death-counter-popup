#include "ui/data_popup/DCPDataPopup.hpp"

#include "ui/data_popup/components/LeftPanel.hpp"
#include "ui/data_popup/components/RightPanel.hpp"

inline constexpr float LEFT_RATIO = 12.f / 5.f;
inline constexpr float RIGHT_RATIO = 12.f / 7.f;

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

  const float leftWidth = contentSize.width / LEFT_RATIO - 15.f;
  const float rightWidth = contentSize.width / RIGHT_RATIO - 15.f;
  const float panelHeight = contentSize.height - 120.f;
  constexpr float controlHeight = 35.f;
  constexpr float panelGap = 15.f;

  this->leftPanel = LeftPanel::create(leftWidth, controlHeight, panelHeight, panelGap);
  this->leftPanel->setAnchorPoint({0.f, 0.5f});
  m_mainLayer->addChildAtPosition(this->leftPanel, Anchor::Left, {10.f, -5.f});

  this->rightPanel = RightPanel::create(rightWidth, controlHeight, panelHeight, panelGap);
  this->rightPanel->setAnchorPoint({1.f, 0.5f});
  m_mainLayer->addChildAtPosition(this->rightPanel, Anchor::Right, {-10.f, -5.f});

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

void DCPDataPopup::onLevelSelected(const std::string& levelID) const {
  if (!this->rightPanel) return;
  this->rightPanel->loadLevelInfo(levelID);
}

void DCPDataPopup::load() const {
  if (!this->leftPanel || !this->loadingCircle) return;

  this->loadingCircle->setVisible(true);
  this->leftPanel->setOnSelectedCallback([this](const std::string& levelID) { onLevelSelected(levelID); });
  this->leftPanel->loadLevelList();
  this->leftPanel->displayLevelList();
  this->loadingCircle->setVisible(false);
}
