#pragma once

#include "utils/Utils.hpp"

using namespace geode::prelude;

struct LevelInfo {
  std::string id;
  std::string name;
  std::string type;
};

template <>
struct matjson::Serialize<LevelInfo> {
  static Value toJson(const LevelInfo& info) {
    return makeObject(
      {
        {"id", info.id},
        {"name", info.name},
        {"type", info.type},
      }
    );
  }

  static Result<LevelInfo> fromJson(const Value& value) {
    LevelInfo info;
    info.id = value["id"].asString().unwrapOr("");
    info.name = value["name"].asString().unwrapOr("");
    info.type = value["type"].asString().unwrapOr("");
    return Ok(info);
  }
};
