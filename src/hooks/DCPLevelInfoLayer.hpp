#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelInfoLayer.hpp>
class $modify(DCPLevelInfoLayer, LevelInfoLayer) {
  bool init(GJGameLevel* level, bool challenge);
};
