#include "handlers/settings/MigrateButtonSetting.hpp"

#include "handlers/DataMigrationHandler.hpp"

Result<std::shared_ptr<SettingV3>> MigrateButtonSetting::parse(
  std::string const& key,
  std::string const& modID,
  matjson::Value const& json
) {
  const auto res = std::make_shared<MigrateButtonSetting>();
  auto root = checkJson(json, "MigrateButtonSetting");

  res->init(key, modID, root);
  res->parseNameAndDescription(root);
  res->parseEnableIf(root);

  root.checkUnknownKeys();
  return root.ok(std::static_pointer_cast<SettingV3>(res));
}

SettingNodeV3* MigrateButtonSetting::createNode(const float width) {
  return MigrateButtonSettingNode::create(
    std::static_pointer_cast<MigrateButtonSetting>(shared_from_this()),
    width
  );
}

bool MigrateButtonSettingNode::init(const std::shared_ptr<MigrateButtonSetting>& setting, const float width) {
  if (!SettingNodeV3::init(setting, width)) {
    return false;
  }

  m_buttonSprite = ButtonSprite::create(
    "Migrate All",
    "goldFont.fnt",
    "GJ_button_01.png",
    .8f
  );
  m_buttonSprite->setScale(.5f);
  m_button = CCMenuItemSpriteExtra::create(
    m_buttonSprite,
    this,
    menu_selector(MigrateButtonSettingNode::onButton)
  );
  this->getButtonMenu()->addChildAtPosition(m_button, Anchor::Center);
  this->getButtonMenu()->setContentWidth(60);
  this->getButtonMenu()->updateLayout();
  this->updateState(nullptr);

  return true;
}

void MigrateButtonSettingNode::updateState(CCNode* invoker) {
  SettingNodeV3::updateState(invoker);

  auto shouldEnable = this->getSetting()->shouldEnable();
  m_button->setEnabled(shouldEnable);
  m_buttonSprite->setCascadeColorEnabled(true);
  m_buttonSprite->setCascadeOpacityEnabled(true);
  m_buttonSprite->setOpacity(shouldEnable ? 255 : 155);
  m_buttonSprite->setColor(shouldEnable ? ccWHITE : ccGRAY);
}

void MigrateButtonSettingNode::onButton(CCObject*) {
  if (DataMigrationHandler::migrateAll()) {
    Notification::create("Migrated all", NotificationIcon::Success)->show();
  } else {
    Notification::create("Already migrated all data", NotificationIcon::Info)->show();
  }
}

MigrateButtonSettingNode* MigrateButtonSettingNode::create(
  const std::shared_ptr<MigrateButtonSetting>& setting, const float width
) {
  const auto ret = new MigrateButtonSettingNode();
  if (ret->init(setting, width)) {
    ret->autorelease();
    return ret;
  }
  delete ret;
  return nullptr;
}

std::shared_ptr<MigrateButtonSetting> MigrateButtonSettingNode::getSetting() const {
  return std::static_pointer_cast<MigrateButtonSetting>(SettingNodeV3::getSetting());
}

$on_mod(Loaded) {
  (void)Mod::get()->registerCustomSettingType("dcp-migrate-button", &MigrateButtonSetting::parse);
}
