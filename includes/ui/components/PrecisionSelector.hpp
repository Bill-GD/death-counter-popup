#pragma once

using namespace geode::prelude;

class PrecisionSelector : public CCNode {
  CCLabelBMFont* m_label = nullptr;
  std::function<void(std::string const&)> m_callback;

  std::vector<std::string> m_values;
  size_t m_index = 0;

  bool init(
    const CCSize& size,
    const std::function<void(std::string const&)>& callback
  );

  void onLeft(CCObject*);
  void onRight(CCObject*);
  void updateLabel();

public:
  static PrecisionSelector* create(
    const CCSize& size,
    const std::function<void(std::string const&)>& callback
  );
};
