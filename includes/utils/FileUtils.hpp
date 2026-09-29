#pragma once

#include "Geode/Geode.hpp"
#include "types/types.hpp"

using namespace geode::prelude;

class FileUtils {
public:
  static bool tryWriteString(const std::filesystem::path& filePath, const std::string& value);
  static bool tryWrite(const std::filesystem::path& filePath, const matjson::Value& value);
  static std::pair<bool, matjson::Value> tryRead(const std::filesystem::path& filePath, bool shouldLog = true);
  static std::pair<bool, std::string> tryReadString(const std::filesystem::path& filePath, bool shouldLog = true);
  static bool tryMove(const std::filesystem::path& oldPath, const std::filesystem::path& newPath);
  static std::vector<std::filesystem::path> getAllFiles(const std::filesystem::path& directoryPath);
  static std::vector<std::filesystem::path> getAllDirectories(const std::filesystem::path& directoryPath);
  static bool tryRemoveDirectory(const std::filesystem::path& directoryPath);

  static int getDirectorySize(const std::filesystem::path& dir);
  static std::filesystem::file_time_type getFileLastWriteTime(const std::filesystem::path& path);
};
