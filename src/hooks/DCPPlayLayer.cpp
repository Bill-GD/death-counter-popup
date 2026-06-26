#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPPlayLayer.hpp>

using namespace geode::prelude;

void DCPPlayLayer::resetLevel() {
  PlayLayer::resetLevel();

  m_fields->runStartPercent = this->getCurrentPercentInt();

  if (const auto level = this->m_level; level->m_levelType == GJLevelType::Main) {
    SaveHandler::currentLevelID = std::to_string(level->m_levelID.value()) + "-local";
    SaveHandler::loadSaveData();
  }
}

void DCPPlayLayer::destroyPlayer(PlayerObject* player, GameObject* object) {
  PlayLayer::destroyPlayer(player, object);

  if (player->m_isDead) {
    this->removeLabel();

    m_fields->label = getPopupLabel();
    this->getChildByID("UILayer")->addChild(m_fields->label);

    m_fields->label->runAction(
      CCSequence::create(
        CCEaseBackOut::create(CCScaleTo::create(0.15f, 1.25f)),
        CCEaseBackOut::create(CCScaleTo::create(0.2f, 0.75f)),
        CCDelayTime::create(1.4f),
        // CCEaseBackOut::create(CCScaleTo::create(0.3f, 0.0f)),
        CCSpawn::create(
          CCFadeOut::create(0.4f),
          CCMoveBy::create(0.4f, CCPoint(0, 20)),
          nullptr
        ),
        CCCallFunc::create(this, callfunc_selector(DCPPlayLayer::removeLabel)),
        nullptr
      )
    );
  }
}

void DCPPlayLayer::removeLabel() {
  if (!m_fields->label) return;

  m_fields->label->removeFromParent();
  m_fields->label = nullptr;
}

CCLabelBMFont* DCPPlayLayer::getPopupLabel() {
  const auto currentPercent = this->getCurrentPercentInt();
  const auto textFmt = m_fields->runStartPercent == 0
                         ? fmt::format("{}", currentPercent)
                         : fmt::format("{}-{}", m_fields->runStartPercent, currentPercent);

  const auto label = CCLabelBMFont::create(textFmt.c_str(), "bigFont.fnt");
  label->setPosition(m_fields->winSize.width * 0.25, m_fields->winSize.height * 0.85);
  label->setRotation(-15);
  label->setScale(0.0f);

  return label;
}
