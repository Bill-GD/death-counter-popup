#include "utils/Utils.hpp"

std::pair<std::string, std::string> Utils::split(std::string str, const char delimiter) {
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

std::pair<std::string, std::string> Utils::computeRunKeys(const std::string& key) {
  if (key.empty()) return {std::string{}, std::string{}};
  if (key.contains('-')) {
    const auto [start, end] = split(key, '-');
    const auto [startLeft, startRight] = split(start, '.');
    const auto [endLeft, endRight] = split(end, '.');

    const int precision = std::max(startRight.size(), endRight.size());
    const auto paddedStartRight = Utils::padToPrecision(startRight, precision);
    const auto paddedEndRight = Utils::padToPrecision(endRight, precision);

    std::string newKey = startLeft;
    if (precision > 0) {
      newKey += "." + paddedStartRight;
    }
    newKey += "-" + endLeft;
    if (precision > 0) {
      newKey += "." + paddedEndRight;
    }

    if (precision > 0) {
      std::string parentStart = startLeft;
      if (paddedStartRight.size() > 1) {
        parentStart += "." + paddedStartRight.substr(0, paddedStartRight.size() - 1);
      }
      std::string parentEnd = endLeft;
      if (paddedEndRight.size() > 1) {
        parentEnd += "." + paddedEndRight.substr(0, paddedEndRight.size() - 1);
      }
      return {newKey, parentStart + "-" + parentEnd};
    }
    return {newKey, ""};
  }

  if (key.contains('.')) {
    const auto [left, right] = split(key, '.');
    std::string parent = left;
    if (right.size() > 1) {
      parent += "." + right.substr(0, right.size() - 1);
    }
    return {key, parent};
  }

  return {key, ""};
}
