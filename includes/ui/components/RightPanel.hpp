#pragma once

using namespace geode::prelude;

class RightPanel : public CCNode {
  CCMenuItemToggler* m_linkingButton = nullptr;
  Label* m_infoLabel = nullptr;
  Label* m_popupStatusLabel = nullptr;
  std::string m_selectedLevelID;
  bool m_isLinking = false;

  void onPlayButtonClicked(CCObject* sender);
  void onLinkingButtonClicked(CCObject* sender);
  void onStatButtonClicked(CCObject* sender);
  void onDeleteButtonClicked(CCObject* sender);

protected:
  bool init(float width, float controlHeight, float infoHeight, float gap);

public:
  static RightPanel* create(float width, float controlHeight, float infoHeight, float gap);
  void loadLevelInfo(std::string levelID);
};
