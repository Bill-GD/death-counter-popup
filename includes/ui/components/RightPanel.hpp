#pragma once

using namespace geode::prelude;

class RightPanel : public CCNode {
  CCMenuItemToggler* m_linkingButton = nullptr;
  SimpleTextArea* m_infoTextArea = nullptr;
  Label* m_popupStatusLabel = nullptr;
  ScrollLayer* m_scrollLayer = nullptr;
  std::string m_selectedLevelID;
  std::string m_firstLinkID;
  bool m_isLinking = false;

  void onPlayButtonClicked(CCObject*);
  void onLinkingButtonClicked(CCObject* sender);
  void onLoadCurrentButtonClicked(CCObject*);
  void onDTButtonClicked(CCObject*);
  void onStatButtonClicked(CCObject*);
  void onDeleteButtonClicked(CCObject*);

protected:
  bool init(float width, float controlHeight, float infoHeight, float gap);

public:
  static RightPanel* create(float width, float controlHeight, float infoHeight, float gap);
  void loadLevelInfo(std::string levelID, bool updateToggle = true);
  void unloadLevelInfo();
};
