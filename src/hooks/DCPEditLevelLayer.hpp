#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/EditLevelLayer.hpp>
class $modify(DCPEditLevelLayer, EditLevelLayer) {
  bool init(GJGameLevel* level);
};
