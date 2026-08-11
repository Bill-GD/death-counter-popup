#pragma once

#include "utils/Utils.hpp"

using namespace geode::prelude;

struct RunData {
  int count = 0;
  std::string parent;
};

template <>
struct matjson::Serialize<RunData> {
  static Value toJson(const RunData& nodeData) {
    return makeObject(
      {
        {"count", nodeData.count},
        {"parent", nodeData.parent},
      }
    );
  }

  static Result<RunData> fromJson(const Value& value) {
    RunData nodeData;
    nodeData.count = value["count"].asInt().unwrapOr(0);
    nodeData.parent = value["parent"].asString().unwrapOr("");
    return Ok(nodeData);
  }
};
