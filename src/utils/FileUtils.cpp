#include "utils/FileUtils.hpp"

bool FileUtils::tryWriteString(const std::filesystem::path& filePath, const std::string& value) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::writeString(filePath, value);
    if (res.isOk()) {
      return true;
    }
    log::warn("Write failed (attempt {}): {}", i + 1, res.unwrapErr());
  }
  return false;
}

bool FileUtils::tryWrite(const std::filesystem::path& filePath, const matjson::Value& value) {
  return tryWriteString(filePath, value.dump(matjson::NO_INDENTATION));
}

std::pair<bool, matjson::Value> FileUtils::tryRead(const std::filesystem::path& filePath, const bool shouldLog) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::readJson(filePath);
    if (res.isOk()) {
      return {true, res.unwrap()};
    }
    if (shouldLog) log::warn("Read {} failed (attempt {}): {}", filePath.string(), i + 1, res.unwrapErr());
  }
  return {false, {}};
}

std::pair<bool, std::string> FileUtils::tryReadString(const std::filesystem::path& filePath, const bool shouldLog) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::readString(filePath);
    if (res.isOk()) {
      return {true, res.unwrap()};
    }
    if (shouldLog) log::warn("Read {} failed (attempt {}): {}", filePath.string(), i + 1, res.unwrapErr());
  }
  return {false, ""};
}

bool FileUtils::tryMove(const std::filesystem::path& oldPath, const std::filesystem::path& newPath) {
  std::error_code ec;
  std::filesystem::rename(oldPath, newPath, ec);

  if (ec) {
    log::error("Failed to move file: {}", ec.message());
    return false;
  }
  return true;
}

std::vector<std::filesystem::path> FileUtils::getAllFiles(const std::filesystem::path& directoryPath) {
  const auto res = file::readDirectory(directoryPath);
  if (res.isErr()) {
    log::warn("Read directory failed: {}", res.unwrapErr());
    return {};
  }
  const auto& files = res.unwrap();
  return ranges::filter(
    files,
    [](auto const& file) {
      return std::filesystem::is_regular_file(file);
    }
  );
}

std::vector<std::filesystem::path> FileUtils::getAllDirectories(const std::filesystem::path& directoryPath) {
  const auto res = file::readDirectory(directoryPath);
  if (res.isErr()) {
    log::warn("Read directory failed: {}", res.unwrapErr());
    return {};
  }
  const auto& files = res.unwrap();
  return ranges::filter(
    files,
    [](auto const& file) {
      return std::filesystem::is_directory(file);
    }
  );
}

bool FileUtils::tryRemoveDirectory(const std::filesystem::path& directoryPath) {
  std::error_code ec;
  const auto success = std::filesystem::remove_all(directoryPath, ec) > 0;
  return !ec && success;
}

int FileUtils::getDirectorySize(const std::filesystem::path& dir) {
  int size = 0;
  std::error_code ec;
  for (const auto& entry : std::filesystem::recursive_directory_iterator(
         dir,
         std::filesystem::directory_options::skip_permission_denied,
         ec
       )) {
    if (entry.is_regular_file(ec)) {
      size += entry.file_size(ec);
    }
  }
  return size;
}

std::filesystem::file_time_type FileUtils::getFileLastWriteTime(const std::filesystem::path& path) {
  if (!is_regular_file(path)) return {};

  std::error_code ec;
  const auto latest = std::filesystem::last_write_time(path, ec);
  if (ec) return {};
  return latest;
}
