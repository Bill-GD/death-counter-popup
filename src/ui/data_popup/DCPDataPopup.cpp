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

  setID("data-viewer"_spr);
  setTitle("Run Counter Data Viewer");

  addContent();

  return true;
}

void DCPDataPopup::addContent() {
  setAnchorPoint({0.5f, 0.5f});

  const auto contentSize = m_mainLayer->getScaledContentSize();

  const float leftWidth = contentSize.width / LEFT_RATIO - 15.f;
  const float rightWidth = contentSize.width / RIGHT_RATIO - 15.f;
  const float panelHeight = contentSize.height - 120.f;
  constexpr float controlHeight = 35.f;
  constexpr float panelGap = 15.f;

  m_leftPanel = LeftPanel::create(leftWidth, controlHeight, panelHeight, panelGap);
  m_leftPanel->setAnchorPoint({0.f, 0.5f});
  m_mainLayer->addChildAtPosition(m_leftPanel, Anchor::Left, {10.f, -5.f});

  m_rightPanel = RightPanel::create(rightWidth, controlHeight, panelHeight, panelGap);
  m_rightPanel->setAnchorPoint({1.f, 0.5f});
  m_mainLayer->addChildAtPosition(m_rightPanel, Anchor::Right, {-10.f, -5.f});

  // const auto loadingLayer = CCLayer::create();
  // loadingLayer->setContentSize(contentSize);
  // loadingLayer->setAnchorPoint({0.5f, 0.5f});
  // loadingLayer->setZOrder(3);
  // m_mainLayer->addChildAtPosition(loadingLayer, Anchor::Center);

  // m_loadingCircle = LoadingCircle::create();
  // m_loadingCircle->setParentLayer(m_mainLayer);
  // m_loadingCircle->setContentSize(contentSize);
  // m_loadingCircle->setColor({200, 200, 200});
  // m_loadingCircle->show();
  // m_loadingCircle->setVisible(false);

  m_mainLayer->updateLayout();
}

void DCPDataPopup::onLevelSelected(const std::string& levelID) const {
  if (!m_rightPanel) return;
  m_rightPanel->loadLevelInfo(levelID);
}

void DCPDataPopup::load() const {
  // if (!m_leftPanel || !m_loadingCircle) return;
  if (!m_leftPanel) return;

  // m_loadingCircle->setVisible(true);
  m_leftPanel->setOnSelectedCallback([this](const std::string& levelID) { onLevelSelected(levelID); });
  m_leftPanel->loadLevelList();
  m_leftPanel->displayLevelList();
  // m_loadingCircle->setVisible(false);
}
