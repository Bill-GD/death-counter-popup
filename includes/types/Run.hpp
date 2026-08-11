#pragma once

#include "utils/Utils.hpp"

using namespace geode::prelude;

struct Run {
private:
  static Run init(const std::string& key) {
    return Run{
      .count = 0,
      .parentKey = Utils::getParentKey(key),
      .parent = nullptr,
    };
  }

public:
  int count = 0;
  std::string parentKey;
  Run* parent = nullptr;

  static Run& init(std::map<std::string, Run>& runs, const std::string& key) {
    if (const auto it = runs.find(key); it != runs.end()) {
      auto& run = it->second;
      if (!run.parentKey.empty() && run.parent == nullptr) {
        run.parent = &init(runs, run.parentKey);
      }
      return run;
    }

    auto [it, inserted] = runs.emplace(key, init(key));
    auto& run = it->second;

    if (!run.parentKey.empty()) {
      run.parent = &init(runs, run.parentKey);
    }

    return run;
  }

  void addDeath() {
    auto* node = this;
    while (node != nullptr && !node->parentKey.empty()) {
      node->count++;
      node = node->parent;
    }
  }
};

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
