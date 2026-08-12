#include "utils/Utils.hpp"

std::pair<std::string, std::string> Utils::split(const std::string& str, const char& delimiter) {
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
