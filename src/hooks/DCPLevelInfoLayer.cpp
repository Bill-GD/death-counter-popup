#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPLevelInfoLayer.hpp>

bool DCPLevelInfoLayer::init(GJGameLevel* level, bool challenge) {
  if (!LevelInfoLayer::init(level, challenge)) {
    return false;
  }

  SaveHandler::currentLevelID = std::to_string(level->m_levelID.value());
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }
  // log::info("Level: id={}, type={}", level->m_levelID, Utils::levelTypeToString(level->m_levelType));

  return true;
}
