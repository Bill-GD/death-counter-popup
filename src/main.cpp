

// #include <Geode/modify/MenuLayer.hpp>
// class $modify(MyMenuLayer, MenuLayer) {
//   bool init() override {
//     if (!MenuLayer::init()) {
//       return false;
//     }
//
//     log::debug("Hello from my MenuLayer::init hook! This layer has {} children.", this->getChildrenCount());
//
//     auto myButton = CCMenuItemSpriteExtra::create(
//       CCSprite::createWithSpriteFrameName("GJ_likeBtn_001.png"),
//       this,
//       menu_selector(MyMenuLayer::onMyButton)
//     );
//
//     auto menu = this->getChildByID("bottom-menu");
//     menu->addChild(myButton);
//     myButton->setID("my-button"_spr);
//     menu->updateLayout();
//
//     return true;
//   }
//
//   void onMyButton(CCObject*) {
//     FLAlertLayer::create(
//       "Geode",
//       fmt::format("{}", Mod::get()->getSavedValue("test-value", 0)),
//       "OK"
//     )->show();
//     // Mod::get()->setSavedValue("test-value", 245543);
//   }
// };
