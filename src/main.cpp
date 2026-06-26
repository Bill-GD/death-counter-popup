#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(HookedPlayLayer, PlayLayer) {
  struct Fields {
    CCSize winSize;
    CCLabelBMFont* label = nullptr;
    Fields() : winSize(CCDirector::get()->getWinSize()) {}
  };

  void destroyPlayer(PlayerObject* player, GameObject* object) override {
    PlayLayer::destroyPlayer(player, object);

    if (player->m_isDead) {
      this->removeLabel();

      m_fields->label = getPopupLabel();
      this->getChildByID("UILayer")->addChild(m_fields->label);

      m_fields->label->runAction(
        CCSequence::create(
          CCEaseBackOut::create(CCScaleTo::create(0.15f, 1.5f)),
          CCEaseBackOut::create(CCScaleTo::create(0.15f, 0.75f)),
          CCDelayTime::create(1.3f),
          // CCEaseBackOut::create(CCScaleTo::create(0.3f, 0.0f)),
          CCSpawn::create(
            CCFadeOut::create(0.4f),
            CCMoveBy::create(0.4f, CCPoint(0, 20)),
            nullptr
          ),
          CCCallFunc::create(this, callfunc_selector(HookedPlayLayer::removeLabel)),
          nullptr
        )
      );
    }
  }

  void removeLabel() {
    if (!m_fields->label) return;

    m_fields->label->removeFromParent();
    m_fields->label = nullptr;
  }

  CCLabelBMFont* getPopupLabel() {
    const auto currentPercent = this->getCurrentPercentInt();

    const auto label = CCLabelBMFont::create(fmt::format("{}", currentPercent).c_str(), "bigFont.fnt");
    label->setPosition(m_fields->winSize.width * 0.15, m_fields->winSize.height * 0.85);
    label->setRotation(-15);
    label->setScale(0.0f);

    return label;
  }
};
