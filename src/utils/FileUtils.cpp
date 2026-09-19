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

std::pair<bool, matjson::Value> FileUtils::tryRead(const std::filesystem::path& filePath) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::readJson(filePath);
    if (res.isOk()) {
      return {true, res.unwrap()};
    }
    log::warn("Read failed (attempt {}): {}", i + 1, res.unwrapErr());
  }
  return {false, {}};
}

std::pair<bool, std::string> FileUtils::tryReadString(const std::filesystem::path& filePath) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::readString(filePath);
    if (res.isOk()) {
      return {true, res.unwrap()};
    }
    log::warn("Read failed (attempt {}): {}", i + 1, res.unwrapErr());
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
