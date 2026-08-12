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
  const auto clampedPercent = std::min(std::max(0.f, percent), maxClamp);
  const auto num = std::pow(10.f, Constants::MAX_PRECISION);
  const auto truncated = std::trunc(clampedPercent * num) / num;
  return fmt::format("{}", truncated);
}

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

  log::info("Stars            : {}", level->m_stars);
  log::info("Orbs             : {}", level->m_orbCompletion);
  log::info("Coins            : {}", level->m_coins);

  log::info("Difficulty       : {}", static_cast<int>(level->m_difficulty));
  log::info("Level Type       : {}", levelTypeToString(level->m_levelType));
  log::info("Length           : {}", level->m_levelLength);
}
