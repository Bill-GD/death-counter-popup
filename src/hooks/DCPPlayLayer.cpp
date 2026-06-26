#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPPlayLayer.hpp>
#include <utils/Utils.hpp>

using namespace geode::prelude;

bool DCPPlayLayer::init(GJGameLevel* level, const bool useReplay, const bool dontCreateObject) {
  if (level->m_levelType == GJLevelType::Main) {
    SaveHandler::currentLevelID = Utils::getLevelID(level);
    SaveHandler::loadSaveData();
  }
  return PlayLayer::init(level, useReplay, dontCreateObject);
}

void DCPPlayLayer::onExit() {
  SaveHandler::saveData();
  PlayLayer::onExit();
}

void DCPPlayLayer::resetLevel() {
  PlayLayer::resetLevel();
  m_fields->isNoclipping = false;
  m_fields->runStartPercent = this->getCurrentPercentInt();
  m_fields->currentAttemptGameObject = nullptr;
}

void DCPPlayLayer::destroyPlayer(PlayerObject* player, GameObject* gameObject) {
  PlayLayer::destroyPlayer(player, gameObject);

  m_fields->currentAttemptGameObject = gameObject;

  if (!player->m_isDead
    && !m_fields->isNoclipping
    && !m_levelEndAnimationStarted
    && m_fields->currentAttemptGameObject != gameObject
  ) {
    m_fields->isNoclipping = true;
  }

  if (player->m_isDead
    && !this->m_level->isPlatformer()
    && !m_fields->isNoclipping
  ) {
    this->removeLabel();

    const auto currentPercent = this->getCurrentPercentInt();
    const auto runLabelStr = m_fields->runStartPercent == 0
                               ? std::to_string(currentPercent)
                               : std::to_string(m_fields->runStartPercent) + "-" + std::to_string(currentPercent);

    SaveHandler::updateDeath(runLabelStr);

    if (Utils::isLevelCompleted(this->m_level)) {
      return;
    }

    m_fields->label = getPopupLabel(runLabelStr);
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

CCLabelBMFont* DCPPlayLayer::getPopupLabel(std::string deathKey) {
  const auto textFmt = fmt::format("{}x{}", deathKey, SaveHandler::deaths.at(deathKey));

  const auto label = CCLabelBMFont::create(textFmt.c_str(), "bigFont.fnt");
  label->setPosition(m_fields->winSize.width * 0.25, m_fields->winSize.height * 0.85);
  label->setRotation(-15);
  label->setScale(0.0f);

  return label;
}
