#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <Geode/Geode.hpp>
#include <handlers/SaveHandler.hpp>
#include <hooks/DCPEditLevelLayer.hpp>

bool DCPEditLevelLayer::init(GJGameLevel* level) {
  if (!EditLevelLayer::init(level)) {
    return false;
  }

  const auto editorID = EditorIDs::getID(level);
  SaveHandler::currentLevelID = std::to_string(editorID) + "-editor";
  if (level->m_levelType != GJLevelType::Main) {
    SaveHandler::loadSaveData();
  }
  // log::info("Level: id={}, type={}", editorID, Utils::levelTypeToString(level->m_levelType));

  return true;
}
