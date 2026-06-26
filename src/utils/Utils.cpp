#include <utils/Utils.hpp>

const char* Utils::levelTypeToString(GJLevelType type) {
  switch (type) {
    case GJLevelType::Main: return "Main";
    case GJLevelType::Editor: return "Editor";
    case GJLevelType::Default: return "Default";
    case GJLevelType::Saved: return "Saved";
    case GJLevelType::SearchResult: return "SearchResult";
    default: return "Unknown";
  }
}
