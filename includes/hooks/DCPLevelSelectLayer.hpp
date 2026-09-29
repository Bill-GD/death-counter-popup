#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

#include "Geode/modify/LevelSelectLayer.hpp"
class $modify(DCPLevelSelectLayer, LevelSelectLayer) {
  bool init(int page);
};
