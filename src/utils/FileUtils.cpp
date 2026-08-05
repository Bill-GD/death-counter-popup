#include "utils/FileUtils.hpp"

bool FileUtils::tryWrite(const std::filesystem::path& filePath, const matjson::Value& value) {
  for (int i = 0; i < 3; ++i) {
    auto res = file::writeString(filePath, value.dump(matjson::NO_INDENTATION));
    if (res.isOk()) {
      return true;
    }
    log::warn("Write failed (attempt {}): {}", i + 1, res.unwrapErr());
  }
  return false;
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
