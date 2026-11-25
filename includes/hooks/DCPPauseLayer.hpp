#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

#include "Geode/modify/PauseLayer.hpp"
class $modify(DCPPauseLayer, PauseLayer) {
  void customSetup() override;
};
