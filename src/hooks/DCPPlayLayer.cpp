#include "hooks/DCPPlayLayer.hpp"

#include "Geode/Geode.hpp"
#include "handlers/SaveHandler.hpp"
#include "handlers/settings/Settings.hpp"
#include "utils/LevelUtils.hpp"

using namespace geode::prelude;

bool DCPPlayLayer::init(GJGameLevel* level, const bool useReplay, const bool dontCreateObject) {
  if (level->m_levelType == GJLevelType::Main) {
    SaveHandler::setLevel(level);
    SaveHandler::loadSaveData();
  }
  m_fields->currentBest = level->m_newNormalPercent2.value();

  const auto res = PlayLayer::init(level, useReplay, dontCreateObject);
  m_fields->runStartPercent = getActualCurrentPercent();
  // m_fields->playerStartPositionX = this->m_player1->getPositionX();
  return res;
}

void DCPPlayLayer::onQuit() {
  SaveHandler::saveData();
  PlayLayer::onQuit();
}

void DCPPlayLayer::resetLevel() {
  PlayLayer::resetLevel();
  m_fields->isNoclipping = false;
  m_fields->runStartPercent = getActualCurrentPercent();
  // m_fields->playerStartPositionX = this->m_player1->getPositionX();
  m_fields->currentAttemptGameObject = nullptr;
}

void DCPPlayLayer::destroyPlayer(PlayerObject* player, GameObject* gameObject) {
  PlayLayer::destroyPlayer(player, gameObject);
  if (this->m_level->isPlatformer()) return;

  m_fields->currentAttemptGameObject = gameObject;

  if (!player->m_isDead
    && !m_fields->isNoclipping
    && !m_levelEndAnimationStarted
    && m_fields->currentAttemptGameObject != gameObject
  ) {
    m_fields->isNoclipping = true;
  }

  if (player->m_isDead && !m_fields->isNoclipping) {
    const auto runLabelStr = getRunLabelString(getActualCurrentPercent(), 99.999900f);
    SaveHandler::incrementRun(runLabelStr);

    if (LevelUtils::isLevelCompleted(this->m_level) && !Settings::isShownForCompleted()) return;

    spawnLabel(LevelUtils::getKeyByPrecision(runLabelStr, Settings::getLabelPrecision()));
    m_fields->currentBest = this->m_level->m_newNormalPercent2.value();
  }
}

void DCPPlayLayer::levelComplete() {
  if (this->m_level->isPlatformer()) {
    PlayLayer::levelComplete();
    return;
  }

  const auto runLabelStr = getRunLabelString(100.f);
  SaveHandler::incrementRun(runLabelStr);

  auto shouldShow = true;
  if (LevelUtils::isLevelCompleted(this->m_level) && !Settings::isShownForCompleted()) shouldShow = false;

  PlayLayer::levelComplete();

  if (m_fields->isNoclipping) return;

  if (shouldShow) spawnLabel(LevelUtils::getKeyByPrecision(runLabelStr, Settings::getLabelPrecision()));
}

void DCPPlayLayer::removeLabel() {
  if (!m_fields->label) return;

  m_fields->label->removeFromParent();
  m_fields->label = nullptr;
}

void DCPPlayLayer::spawnLabel(const std::string& labelStr) {
  if (!Settings::isEnabled()) return;

  this->removeLabel();
  m_fields->label = getPopupLabel(labelStr);
  this->getChildByID("UILayer")->addChild(m_fields->label);

  log::info("Spawned label at ({}), {}°", m_fields->label->getPosition(), m_fields->label->getRotation());

  m_fields->label->runAction(getPopupSequence(true));
  m_fields->label->getChildByID("RunLabel")->runAction(getPopupSequence(false));
  m_fields->label->getChildByID("CountLabel")->runAction(getPopupSequence(false));
}

CCNode* DCPPlayLayer::getPopupLabel(const std::string& deathKey) {
  const auto isRun = deathKey.contains('-');
  const auto isNewBest = !isRun && this->getCurrentPercentInt() > m_fields->currentBest;
  const auto useGoldFont = isNewBest && Settings::isNewBestGolden();

  const auto labelScale = Settings::getScale() + (useGoldFont ? 0.35f : 0.f);

  const auto runLabel = Label::create(
    deathKey,
    useGoldFont ? "goldFont.fnt" : "bigFont.fnt"
  );
  runLabel->setID("RunLabel");
  runLabel->setOpacity(0.f);
  runLabel->setAnchorPoint({0.f, 0.f});
  runLabel->setScale(labelScale);

  const auto countLabel = Label::create(
    fmt::format("x{}", SaveHandler::deaths.at(deathKey).count),
    useGoldFont ? "goldFont.fnt" : "bigFont.fnt"
  );
  countLabel->setID("CountLabel");
  countLabel->setOpacity(0.f);
  countLabel->setAnchorPoint({0.f, 0.f});
  countLabel->setScale(labelScale * 0.7f);

  const auto labelNode = CCNode::create();

  labelNode->addChild(runLabel);
  labelNode->addChild(countLabel);

  runLabel->setPosition(0.f, 0.f);
  countLabel->setPosition(
    runLabel->getScaledContentWidth(),
    0.f
  );

  labelNode->setContentSize(
    {
      runLabel->getScaledContentWidth() + countLabel->getScaledContentWidth(),
      std::max(
        runLabel->getScaledContentHeight(),
        countLabel->getScaledContentHeight()
      )
    }
  );

  labelNode->setID("RunCounterLabel");
  labelNode->setPosition(Settings::getLabelPosition());
  labelNode->setRotation(static_cast<float>(Settings::getRotation()));
  labelNode->setScale(0.f);
  labelNode->setAnchorPoint({0.5f, 0.5f});

  return labelNode;
}

CCSequence* DCPPlayLayer::getPopupSequence(const bool isParent) {
  const std::string popupStyle = Settings::getPopupStyle();

  if (popupStyle == POPUP_STYLE_ANIMATED) {
    if (isParent) {
      return CCSequence::create(
        CCEaseBackOut::create(CCScaleTo::create(0.15f, 1.25f)),
        CCEaseBackOut::create(CCScaleTo::create(0.2f, m_fields->endScale)),
        CCDelayTime::create(1.85f),
        CCScaleTo::create(0.25f, 0.f),
        CCCallFunc::create(this, callfunc_selector(DCPPlayLayer::removeLabel)),
        nullptr
      );
    }
    return CCSequence::create(
      CCFadeTo::create(0.f, Settings::getOpacity()),
      CCDelayTime::create(2.5f),
      nullptr
    );
  }

  if (popupStyle == POPUP_STYLE_FADE) {
    if (isParent) {
      return CCSequence::create(
        CCScaleTo::create(0.f, m_fields->endScale),
        CCDelayTime::create(2.5f),
        CCCallFunc::create(this, callfunc_selector(DCPPlayLayer::removeLabel)),
        nullptr
      );
    }
    return CCSequence::create(
      CCFadeTo::create(0.15f, Settings::getOpacity()),
      CCDelayTime::create(1.95f),
      CCFadeTo::create(0.4f, 0),
      nullptr
    );
  }

  // flat or invalid
  if (isParent) {
    return CCSequence::create(
      CCScaleTo::create(0.f, m_fields->endScale),
      CCDelayTime::create(2.5f),
      CCCallFunc::create(this, callfunc_selector(DCPPlayLayer::removeLabel)),
      nullptr
    );
  }
  return CCSequence::create(
    CCFadeTo::create(0.f, Settings::getOpacity()),
    CCDelayTime::create(2.5f),
    nullptr
  );
}

std::string DCPPlayLayer::getRunLabelString(const float& currentPercent, const float& maxClamp) {
  std::string labelStr;
  if (m_fields->runStartPercent > 0.f) {
    labelStr = LevelUtils::formatPercent(m_fields->runStartPercent, maxClamp) + "-";
  }
  labelStr += LevelUtils::formatPercent(currentPercent, maxClamp);
  return labelStr;
}

// std::string DCPPlayLayer::getRunByPosition(const float& maxClamp, const bool& useEndTrigger) {
//   const auto endWall = LevelUtils::getEndWall(this);
//   const auto endTrigger = LevelUtils::getLastEndTrigger(this);
//   const bool canUseEndTrigger = useEndTrigger && endTrigger != nullptr;
//   const auto endPositionX = canUseEndTrigger ? endTrigger->getPositionX() : endWall->getPositionX();
//   const auto playerDeathPosition = this->m_player1->getPositionX();
//
//   const float runStartPercent = m_fields->playerStartPositionX / endPositionX * 100;
//   const float runEndPercent = playerDeathPosition / endPositionX * 100;
//
//   std::string labelStr;
//   if (m_fields->playerStartPositionX > 0) {
//     labelStr = LevelUtils::formatPercent(runStartPercent, maxClamp) + "-";
//   }
//   labelStr += LevelUtils::formatPercent(runEndPercent, maxClamp);
//   return labelStr;
// }

// shout out to eclipse mod (i think) for figuring out timestamp is level frame count (240)
// which solves the issue when level has end trigger
float DCPPlayLayer::getActualCurrentPercent() {
  const auto game = GJBaseGameLayer::get();
  float percent;

  if (game->m_level->m_timestamp > 0) {
    percent = static_cast<float>(game->m_gameState.m_levelTime * 240.f) / game->m_level->m_timestamp * 100.f;
  } else {
    percent = game->m_player1->getPositionX() / game->m_levelLength * 100.f;
  }
  return percent;
}
