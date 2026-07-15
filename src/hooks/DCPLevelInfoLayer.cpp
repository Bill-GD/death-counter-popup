#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPLevelInfoLayer.hpp>
// #include <utils/Utils.hpp>

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  SaveHandler::setLevel(level);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  // Utils::dumpLevelInfo(level);

  return true;
}
