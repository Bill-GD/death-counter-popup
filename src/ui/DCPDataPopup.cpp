#include "ui/DCPDataPopup.hpp"

#include "ui/components/LeftPanel.hpp"
#include "ui/components/RightPanel.hpp"

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

  setID("all-data-viewer");
  setTitle("Run Counter Data Viewer");

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

  m_mainLayer->updateLayout();

  return true;
}

void DCPDataPopup::onLevelSelected(const std::string& levelID) const {
  if (!m_rightPanel) return;
  m_rightPanel->loadLevelInfo(levelID);
}

void DCPDataPopup::load() const {
  if (!m_leftPanel) return;

  m_leftPanel->setOnSelectedCallback([this](const std::string& levelID) { onLevelSelected(levelID); });
  m_leftPanel->loadLevelList();
  m_leftPanel->displayLevelList();
}
