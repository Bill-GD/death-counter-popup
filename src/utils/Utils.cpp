#include <cvolton.level-id-api/include/EditorIDs.hpp>
#include <utils/Utils.hpp>

const char* Utils::levelTypeToString(const GJLevelType type) {
  switch (type) {
    case GJLevelType::Main: return "Main";
    case GJLevelType::Editor: return "Editor";
    case GJLevelType::Default: return "Default";
    case GJLevelType::Saved: return "Saved";
    case GJLevelType::SearchResult: return "SearchResult";
    default: return "Unknown";
  }
}

bool Utils::isLevelCompleted(GJGameLevel* level) {
  return level->m_levelType == GJLevelType::Saved
    && level->m_newNormalPercent2.value() == 100;
}

bool Utils::isGauntletLevel(GJGameLevel* level) {
  return level->m_gauntletLevel;
}

bool Utils::isDailyLevel(GJGameLevel* level) {
  return level->m_dailyID > 0;
}

std::string Utils::getLevelID(GJGameLevel* level) {
  auto levelID = std::to_string(level->m_levelID.value());

  switch (level->m_levelType) {
    case GJLevelType::Editor: {
      const auto editorID = EditorIDs::getID(level);
      return std::to_string(editorID) + "-editor";
    }
    case GJLevelType::Main: {
      levelID += "-local";
      break;
    }
    case GJLevelType::Default:
    case GJLevelType::SearchResult:
    case GJLevelType::Saved: {
      if (isDailyLevel(level)) levelID += "-daily";
      else if (isGauntletLevel(level)) levelID += "-gauntlet";
      break;
    }
  }
  return levelID;
}
