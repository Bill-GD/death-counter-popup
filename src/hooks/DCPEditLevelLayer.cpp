#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/Geode.hpp>
#include <hooks/DCPEditLevelLayer.hpp>
#include <utils/Utils.hpp>

bool DCPEditLevelLayer::init(GJGameLevel* level) {
  if (!EditLevelLayer::init(level)) {
    return false;
  }

  const auto editorID = EditorIDs::getID(level);
  log::info("Level: id={}, type={}", editorID, Utils::levelTypeToString(level->m_levelType));

  return true;
}
