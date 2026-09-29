#include "utils/Utils.hpp"

std::pair<std::string, std::string> Utils::splitOnce(const std::string& str, const char& delimiter) {
  const auto pos = str.find(delimiter);
  const auto first = str.substr(0, pos);
  const auto second = pos == std::string::npos ? "" : str.substr(pos + 1);

  return {first, second};
}

std::string Utils::padToPrecision(std::string str, const int len) {
  if (str.size() < len) {
    str.append(len - str.size(), '0');
  }
  return str;
}

std::string Utils::formatSizeString(int byte) {
  const std::vector<std::string> suffixes = {"B", "KB", "MB"};
  float result = static_cast<float>(byte);
  size_t i = 0;

  for (; i < suffixes.size() - 1; ++i) {
    if (result < 1024.f) break;
    result /= 1024.f;
  }
  return fmt::format("{:.2f} {}", result, suffixes[i]);
}
