#include "utils/Utils.hpp"

std::pair<std::string, std::string> Utils::split(std::string str, const char delimiter) {
  const auto pos = str.find(delimiter);
  const auto first = str.substr(0, pos);
  const auto second = str.substr(pos + 1);

  return {first, second};
}
