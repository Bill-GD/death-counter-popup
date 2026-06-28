#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPLevelInfoLayer.hpp>
#include <utils/Utils.hpp>

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  SaveHandler::setLevel(Utils::getLevelID(level), level->m_levelName);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  return true;
}
