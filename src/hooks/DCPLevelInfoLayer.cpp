#include "hooks/DCPLevelInfoLayer.hpp"

#include "Geode/Geode.hpp"
#include "handlers/SaveHandler.hpp"
// #include "utils/LevelUtils.hpp"

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  SaveHandler::setLevel(level);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  // LevelUtils::dumpLevelInfo(level);

  return true;
}
