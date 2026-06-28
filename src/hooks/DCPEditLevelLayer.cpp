#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPEditLevelLayer.hpp>
#include <utils/Utils.hpp>

bool DCPEditLevelLayer::init(GJGameLevel* level) {
  if (!EditLevelLayer::init(level)) {
    return false;
  }

  SaveHandler::setLevel(Utils::getLevelID(level), level->m_levelName);
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }

  return true;
}
