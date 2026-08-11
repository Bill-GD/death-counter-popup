#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class Utils {
public:
  template <typename T>
  static T tryParse(const matjson::Value& value) {
    const auto parseRes = value.as<T>();
    if (parseRes.isOk()) {
      return parseRes.unwrap();
    }
    log::warn("Failed to parse as '{}': {}", typeid(T).name(), parseRes.unwrapErr());
    return T{};
  }

  static std::pair<std::string, std::string> split(std::string str, const char delimiter);
  static std::string padToPrecision(std::string str, int len);
  static std::pair<std::string, std::string> computeRunKeys(const std::string& key);
  static std::vector<std::string> getAllParentKeys(const std::string& key);
};
