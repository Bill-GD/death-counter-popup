#include <Geode/Geode.hpp>
#include <hooks/DCPLevelInfoLayer.hpp>
#include <utils/Utils.hpp>

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  log::info("Level: id={}, type={}", level->m_levelID, Utils::levelTypeToString(level->m_levelType));

  return true;
}
