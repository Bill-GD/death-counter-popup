#include <handlers/SaveHandler.hpp>

using namespace geode::prelude;

$execute {
  (void)file::createDirectory(SaveHandler::savePath);
}
