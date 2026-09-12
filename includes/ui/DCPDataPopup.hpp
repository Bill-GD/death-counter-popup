#pragma once

using namespace geode::prelude;

class DCPDataPopup : public Popup {
protected:
  bool init();

public:
  static DCPDataPopup* create();

private:
  CCNode* getContent();
};
