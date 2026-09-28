#pragma once

#include "utils/Utils.hpp"

using namespace geode::prelude;

struct LevelInfo {
  std::string id;
  std::string name;
};

template <>
struct matjson::Serialize<LevelInfo> {
  static Value toJson(const LevelInfo& info) {
    return makeObject(
      {
        {"id", info.id},
        {"name", info.name},
      }
    );
  }

  static Result<LevelInfo> fromJson(const Value& value) {
    LevelInfo info;
    info.id = value["id"].asString().unwrapOr("");
    info.name = value["name"].asString().unwrapOr("");
    return Ok(info);
  }
};
