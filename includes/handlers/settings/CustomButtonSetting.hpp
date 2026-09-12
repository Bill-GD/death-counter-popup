#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

inline constexpr std::string_view MIGRATE_SETTING_KEY = "migrate-all";

class CustomButtonSetting : public SettingV3 {
public:
  static Result<std::shared_ptr<SettingV3>> parse(
    std::string const& key,
    std::string const& modID,
    matjson::Value const& json
  );

  bool load(matjson::Value const& json) override { return true; }
  bool save(matjson::Value& json) const override { return true; }
  bool isDefaultValue() const override { return true; }
  void reset() override {}

  SettingNodeV3* createNode(float width) override;
};

class CustomButtonSettingNode : public SettingNodeV3 {
protected:
  ButtonSprite* m_buttonSprite;
  CCMenuItemSpriteExtra* m_button;

  bool init(const std::shared_ptr<CustomButtonSetting>& setting, float width);

  void updateState(CCNode* invoker) override;
  void onButton(CCObject*);

  void onCommit() override {}
  void onResetToDefault() override {}

public:
  static CustomButtonSettingNode* create(const std::shared_ptr<CustomButtonSetting>& setting, float width);
  std::shared_ptr<CustomButtonSetting> getSetting() const;

  bool hasUncommittedChanges() const override { return false; }
  bool hasNonDefaultValue() const override { return false; }
};
