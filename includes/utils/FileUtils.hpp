#pragma once

#include "Geode/Geode.hpp"

using namespace geode::prelude;

class FileUtils {
public:
  static bool tryWrite(const std::filesystem::path& filePath, const matjson::Value& value);
  static std::pair<bool, matjson::Value> tryRead(const std::filesystem::path& filePath);
  static bool tryMove(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
  static std::vector<std::filesystem::path> getAllFiles(const std::filesystem::path& directoryPath);
};
