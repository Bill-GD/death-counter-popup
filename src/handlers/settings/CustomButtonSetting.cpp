#include "handlers/settings/CustomButtonSetting.hpp"

#include "handlers/DataMigrationHandler.hpp"

const std::map<std::string_view, std::string> buttonTextMap = {
  {MIGRATE_SETTING_KEY, "Migrate All"}
};

const std::map<std::string_view, std::function<void()>> actionMap = {
  {
    MIGRATE_SETTING_KEY, [] {
      if (DataMigrationHandler::migrateAll()) {
        Notification::create("Migrated all", NotificationIcon::Success)->show();
      } else {
        Notification::create("Already migrated all data", NotificationIcon::Info)->show();
      }
    }
  }
};

Result<std::shared_ptr<SettingV3>> CustomButtonSetting::parse(
  std::string const& key,
  std::string const& modID,
  matjson::Value const& json
) {
  const auto res = std::make_shared<CustomButtonSetting>();
  auto root = checkJson(json, "MigrateButtonSetting");

  res->init(key, modID, root);
  res->parseNameAndDescription(root);
  res->parseEnableIf(root);

  root.checkUnknownKeys();
  return root.ok(std::static_pointer_cast<SettingV3>(res));
}

SettingNodeV3* CustomButtonSetting::createNode(const float width) {
  return CustomButtonSettingNode::create(
    std::static_pointer_cast<CustomButtonSetting>(shared_from_this()),
    width
  );
}

bool CustomButtonSettingNode::init(const std::shared_ptr<CustomButtonSetting>& setting, const float width) {
  if (!SettingNodeV3::init(setting, width)) {
    return false;
  }

  const auto& settingKey = this->getSetting()->getKey();
  m_buttonSprite = ButtonSprite::create(
    buttonTextMap.at(settingKey).c_str(),
    "goldFont.fnt",
    "GJ_button_01.png",
    .8f
  );
  m_buttonSprite->setScale(.5f);
  m_button = CCMenuItemSpriteExtra::create(
    m_buttonSprite,
    this,
    menu_selector(CustomButtonSettingNode::onButton)
  );
  this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
  this->getButtonMenu()->setContentWidth(60);
  this->getButtonMenu()->updateLayout();
  this->updateState(nullptr);

  return true;
}

void CustomButtonSettingNode::updateState(CCNode* invoker) {
  SettingNodeV3::updateState(invoker);

  const auto shouldEnable = this->getSetting()->shouldEnable();
  m_button->setEnabled(shouldEnable);
  m_buttonSprite->setCascadeColorEnabled(true);
  m_buttonSprite->setCascadeOpacityEnabled(true);
  m_buttonSprite->setOpacity(shouldEnable ? 255 : 155);
  m_buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
}

void CustomButtonSettingNode::onButton(CCObject*) {
  const auto& settingKey = this->getSetting()->getKey();
  actionMap.at(settingKey)();
}

CustomButtonSettingNode* CustomButtonSettingNode::create(
  const std::shared_ptr<CustomButtonSetting>& setting, const float width
) {
  const auto ret = new CustomButtonSettingNode();
  if (ret->init(setting, width)) {
    ret->autorelease();
    return ret;
  }
  delete ret;
  return nullptr;
}

std::shared_ptr<CustomButtonSetting> CustomButtonSettingNode::getSetting() const {
  return std::static_pointer_cast<CustomButtonSetting>(SettingNodeV3::getSetting());
}

$on_mod(Loaded) {
  (void)Mod::get()->registerCustomSettingType("dcp-setting-button", &CustomButtonSetting::parse);
}
