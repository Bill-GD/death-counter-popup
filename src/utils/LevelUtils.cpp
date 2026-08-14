#include "utils/LevelUtils.hpp"

#include "cvolton.level-id-api/include/EditorIDs.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

const char* LevelUtils::levelTypeToString(const GJLevelType type) {
  switch (type) {
    case GJLevelType::Main: return "Main";
    case GJLevelType::Editor: return "Editor";
    case GJLevelType::Default: return "Default";
    case GJLevelType::Saved: return "Saved";
    case GJLevelType::SearchResult: return "SearchResult";
    default: return "Unknown";
  }
}

std::string LevelUtils::formatPercent(const float& percent, const float& maxClamp) {
  const auto clampedPercent = std::clamp(percent, 0.f, maxClamp);
  const auto num = std::pow(10.f, Constants::MAX_PRECISION);
  const auto truncated = std::trunc(clampedPercent * num) / num;
  return fmt::format("{:.{}f}", truncated, Constants::MAX_PRECISION);
}

/**
 * @return Pair: 1st - formatted run key, 2nd - parent key
 */
std::pair<std::string, std::string> LevelUtils::computeRunKeys(const std::string& key) {
  if (key.empty()) return {"", ""};
  // filters negative progress (still dont know how they exist)
  if (key.contains('-') && !key.starts_with('-')) {
    const auto [start, end] = Utils::split(key, '-');
    const auto [startLeft, startRight] = Utils::split(start, '.');
    const auto [endLeft, endRight] = Utils::split(end, '.');

    const int precision = std::max(startRight.size(), endRight.size());
    const auto paddedStartRight = Utils::padToPrecision(startRight, precision);
    const auto paddedEndRight = Utils::padToPrecision(endRight, precision);

    std::string newKey = startLeft;
    if (precision > 0) {
      newKey += "." + paddedStartRight;
    }
    newKey += "-" + endLeft;
    if (precision > 0) {
      newKey += "." + paddedEndRight;
    }

    if (precision > 0) {
      std::string parentStart = startLeft;
      if (paddedStartRight.size() > 1) {
        parentStart += "." + paddedStartRight.substr(0, paddedStartRight.size() - 1);
      }
      std::string parentEnd = endLeft;
      if (paddedEndRight.size() > 1) {
        parentEnd += "." + paddedEndRight.substr(0, paddedEndRight.size() - 1);
      }
      return {newKey, parentStart + "-" + parentEnd};
    }
    return {newKey, ""};
  }

  if (key.contains('.')) {
    const auto [left, right] = Utils::split(key, '.');
    std::string parent = left;
    if (right.size() > 1) {
      parent += "." + right.substr(0, right.size() - 1);
    }
    return {key, parent};
  }

  return {key, ""};
}

std::string LevelUtils::getParentKey(const std::string& key) {
  if (key.empty()) return "";
  if (key.contains('-') && !key.starts_with('-')) {
    const auto [start, end] = Utils::split(key, '-');
    const auto [startLeft, startRight] = Utils::split(start, '.');
    const auto [endLeft, endRight] = Utils::split(end, '.');

    const int precision = std::max(startRight.size(), endRight.size());
    const auto paddedStartRight = Utils::padToPrecision(startRight, precision);
    const auto paddedEndRight = Utils::padToPrecision(endRight, precision);

    if (precision > 0) {
      std::string parentStart = startLeft;
      if (paddedStartRight.size() > 1) {
        parentStart += "." + paddedStartRight.substr(0, paddedStartRight.size() - 1);
      }
      std::string parentEnd = endLeft;
      if (paddedEndRight.size() > 1) {
        parentEnd += "." + paddedEndRight.substr(0, paddedEndRight.size() - 1);
      }
      return parentStart + "-" + parentEnd;
    }
    return "";
  }

  if (key.contains('.')) {
    const auto [left, right] = Utils::split(key, '.');
    std::string parent = left;
    if (right.size() > 1) {
      parent += "." + right.substr(0, right.size() - 1);
    }
    return parent;
  }

  return "";
}

std::vector<std::string> LevelUtils::getAllParentKeys(const std::string& key) {
  if (key.empty()) return {};

  if (key.contains('-') && !key.starts_with('-')) {
    const auto [start, end] = Utils::split(key, '-');
    const auto [startLeft, startRight] = Utils::split(start, '.');
    const auto [endLeft, endRight] = Utils::split(end, '.');

    const int precision = std::max(startRight.size(), endRight.size());
    const auto paddedStartRight = Utils::padToPrecision(startRight, precision);
    const auto paddedEndRight = Utils::padToPrecision(endRight, precision);

    std::vector list = {startLeft + "-" + endLeft};

    int i = 1;
    while (i < precision) {
      std::string parentKey = startLeft;
      parentKey += "." + paddedStartRight.substr(0, paddedStartRight.size() - precision + i);
      parentKey += "-" + endLeft;
      parentKey += "." + paddedEndRight.substr(0, paddedEndRight.size() - precision + i);
      list.push_back(parentKey);
      i++;
    }

    return list;
  }

  if (key.contains('.')) {
    const auto [left, right] = Utils::split(key, '.');
    std::vector list = {left};
    const int precision = right.size();

    int i = 1;
    while (i < precision) {
      list.push_back(left + "." + right.substr(0, right.size() - precision + i));
      i++;
    }
    return list;
  }

  return {};
}

std::string LevelUtils::getKeyByPrecision(const std::string& key, const int& precision) {
  auto allKeys = getAllParentKeys(key);
  allKeys.push_back(key);
  const int maxAvailable = static_cast<int>(allKeys.size()) - 1;
  const int maxPrecision = std::min(Constants::MAX_PRECISION, maxAvailable);
  const int clampedPrecision = std::clamp(precision, 0, maxPrecision);
  return allKeys.at(clampedPrecision);
}

int LevelUtils::getKeyPrecision(const std::string& key) {
  std::string part = key;

  if (key.contains('-') && !key.starts_with('-')) {
    part = Utils::split(key, '-').first;
  }
  if (!part.contains('.')) { return 0; }

  const auto [_, right] = Utils::split(part, '.');
  return right.size();
}

bool LevelUtils::isLevelCompleted(GJGameLevel* level) {
  return level->m_newNormalPercent2.value() == 100;
}

bool LevelUtils::isGauntletLevel(const GJGameLevel* level) {
  return level->m_gauntletLevel;
}

bool LevelUtils::isDailyLevel(GJGameLevel* level) {
  return level->m_dailyID > 0;
}

std::string LevelUtils::getLevelID(GJGameLevel* level) {
  auto levelID = std::to_string(level->m_levelID.value());

  switch (level->m_levelType) {
    case GJLevelType::Editor: {
      // auto original = level->m_originalLevel;
      const auto editorID = EditorIDs::getID(level);
      // return original.value() > 0
      //          ? std::to_string(original.value())
      //          : std::to_string(editorID) + "-editor";
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

bool LevelUtils::isModLoaded(const std::string& modID) {
  return Loader::get()->isModLoaded(modID);
}

// EndTriggerGameObject* LevelUtils::getLastEndTrigger(const PlayLayer* layer) {
//   std::vector<EndTriggerGameObject*> endTriggers = {};
//   for (const auto& child : layer->m_objects->asExt<GameObject*>()) {
//     if (auto endTrigger = typeinfo_cast<EndTriggerGameObject*>(child)) {
//       endTriggers.push_back(endTrigger);
//     }
//   }
//   return endTriggers.empty() ? nullptr : endTriggers.back();
// }
//
// EndPortalObject* LevelUtils::getEndWall(const PlayLayer* layer) {
//   for (const auto& child : layer->m_objects->asExt<GameObject*>()) {
//     if (auto endPortal = typeinfo_cast<EndPortalObject*>(child)) {
//       return endPortal;
//     }
//   }
//   log::warn("No endwall found in level, this should not happen");
//   return nullptr;
// }

void LevelUtils::dumpLevelInfo(GJGameLevel* level) {
  log::info("===== GJGameLevel =====");

  log::info("ID               : {}", level->m_levelID);
  log::info("Name             : {}", level->m_levelName);
  log::info("Original         : {}", level->m_originalLevel);
  log::info("Creator          : {}", level->m_creatorName);
  log::info("Creator ID       : {}", level->m_userID);

  log::info("Attempts         : {}", level->m_attempts);
  log::info("Jumps            : {}", level->m_jumps);
  log::info("Clicks           : {}", level->m_clicks);
  log::info("Normal %         : {}", level->m_normalPercent);
  log::info("Practice %       : {}", level->m_practicePercent);
  log::info("New Normal %     : {}", level->m_newNormalPercent2);
  log::info("Working Time 1   : {}", level->m_workingTime);
  log::info("Working Time 2   : {}", level->m_workingTime2);
  log::info("Attempt Time     : {}", level->m_attemptTime);
  // log::info("Timestamp        : {}", level->m_timestamp);

  log::info("Stars            : {}", level->m_stars);
  log::info("Orbs             : {}", level->m_orbCompletion);
  log::info("Coins            : {}", level->m_coins);

  log::info("Difficulty       : {}", static_cast<int>(level->m_difficulty));
  log::info("Level Type       : {}", levelTypeToString(level->m_levelType));
  log::info("Length           : {}", level->m_levelLength);
}

// void LevelUtils::dumpAttemptInfo() {
//   const auto playLayer = PlayLayer::get();
//   const auto game = GJBaseGameLayer::get();
//   const auto level = playLayer->m_level;
//   const auto levelID = getLevelID(level);
//   const auto name = level->m_levelName;
//   const auto startPos = game->m_startPosObject;
//   const auto endTrigger = getLastEndTrigger(playLayer);
//   const auto endWall = getEndWall(playLayer);
//
//   log::info("");
//   log::info("======= AttemptInfo =======");
//   log::info("level id                       : {}", levelID);
//   log::info("level name                     : {}", level->m_levelName);
//   log::info("player pos                     : {}", game->m_player1->getPositionX());
//   log::info("startpos                       : {}", startPos != nullptr ? startPos->getPositionX() : 0);
//   log::info("endTrigger                     : {}", endTrigger != nullptr ? endTrigger->getPositionX() : 0);
//   log::info("endWall                        : {}", endWall != nullptr ? endWall->getPositionX() : 0);
//   log::info("level time                     : {}", game->m_gameState.m_levelTime);
//   log::info("total time                     : {}", game->m_gameState.m_totalTime);
//   log::info("timestamp                      : {}", game->m_level->m_timestamp);
//   log::info("level length                   : {}", game->m_levelLength);
//   log::info(
//     "currentPercent                 : {} ({})",
//     playLayer->getCurrentPercent(),
//     formatPercent(playLayer->getCurrentPercent())
//   );
//
//   if (game->m_level->m_timestamp > 0) {
//     log::info(
//       "eclipse percent (timestamp > 0): {}",
//       static_cast<float>(game->m_gameState.m_levelTime * 240.f) / game->m_level->m_timestamp * 100.f
//     );
//   } else {
//     log::info(
//       "eclipse percent                : {}",
//       game->m_player1->getPositionX() / game->m_levelLength * 100.f
//     );
//   }
//   log::info("======= AttemptInfo =======");
//   log::info("");
// }
