#include "ui/data_popup/components/LeftPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "ui/data_popup/components/LevelTile.hpp"
#include "utils/FileUtils.hpp"

LeftPanel* LeftPanel::create(const CCSize& size) {
  const auto ret = new LeftPanel();
  if (ret->init(size)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool LeftPanel::init(const CCSize& size) {
  if (!CCScale9Sprite::initWithFile("geode.loader/GE_square01.png")) return false;

  this->setContentSize(size);

  const auto scroll = ScrollLayer::create(size - 2.5f);
  scroll->m_contentLayer->setContentSize({size.width, size.height - 4.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  this->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scroll->getContentSize() / 2.f
  );
  this->scrollLayer = scroll;

  return true;
}

void LeftPanel::loadLevelList(const std::function<void(std::string)>& onSelected) const {
  const std::vector<std::filesystem::path> allLevelDirs = FileUtils::getAllDirectories(SaveHandler::PATH);
  const auto allLevelIDs = ranges::filter<std::vector<std::string>>(
    ranges::map<std::vector<std::string>>(
      allLevelDirs,
      [](auto const& dir) {
        return dir.stem().string();
      }
    ),
    [](auto const& id) {
      return id != "backups";
    }
  );

  for (auto const& id : allLevelIDs) {
    this->scrollLayer->m_contentLayer->addChild(
      LevelTile::create(
        id,
        {this->getContentWidth(), 30.f},
        onSelected
      )
    );
  }
  this->scrollLayer->m_contentLayer->updateLayout();
  this->scrollLayer->scrollToTop();
}
