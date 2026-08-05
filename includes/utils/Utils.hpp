#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class Utils {
public:
  static const char* levelTypeToString(GJLevelType type);
  static std::string formatPercent(const float& percent);

  static bool isLevelCompleted(GJGameLevel* level);
  static bool isGauntletLevel(const GJGameLevel* level);
  static bool isDailyLevel(GJGameLevel* level);

  static std::string getLevelID(GJGameLevel* level);

  static bool isModLoaded(const std::string& modID);

  template <typename T>
  static T tryParse(const matjson::Value& value) {
    const auto parseRes = value.as<T>();
    if (parseRes.isOk()) {
      return parseRes.unwrap();
    }
    log::warn("Failed to parse as '{}': {}", typeid(T).name(), parseRes.unwrapErr());
    return T{};
  }

  static void dumpLevelInfo(GJGameLevel* level);
};
