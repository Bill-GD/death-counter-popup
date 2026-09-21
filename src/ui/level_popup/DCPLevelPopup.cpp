#include "ui/level_popup/DCPLevelPopup.hpp"

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
  if (!Popup::init(50.f, 50.f, "geode.loader/GE_square01.png")) {
    return false;
  }

  this->setTitle(fmt::format("Run Counter ({})", level.m_levelName));

  return true;
}
