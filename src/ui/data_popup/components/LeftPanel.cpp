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

  const auto scroll = ScrollLayer::create({size.width - 10.f, size.height - 2.5f});
  const auto scrollSize = scroll->getContentSize();
  scroll->m_contentLayer->setContentSize({scrollSize.width, scrollSize.height - 10.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  const auto scrollbar = Scrollbar::create(scroll);
  scrollbar->setAnchorPoint({1.f, 0.5f});
  scrollbar->setContentSize({4.f, size.height});

  this->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scrollSize / 2.f - CCSize{3.f, 0}
  );
  this->addChildAtPosition(scrollbar, Anchor::Right, {-4.f, 0});

  this->scrollLayer = scroll;

  return true;
}

void LeftPanel::loadLevelList() {
  const std::vector<std::filesystem::path> allLevelDirs = FileUtils::getAllDirectories(SaveHandler::PATH);
  allLevelIDs = ranges::filter(
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
  filteredLevelIDs = allLevelIDs;
}

void LeftPanel::filterByName(std::string input) {
  filteredLevelIDs = ranges::filter(
    allLevelIDs,
    [input](auto const& id) {
      return id.contains(input);
    }
  );
}

void LeftPanel::displayLevelList(const std::function<void(std::string)>& onSelected) const {
  this->scrollLayer->m_contentLayer->removeAllChildren();

  for (auto const& id : filteredLevelIDs) {
    this->scrollLayer->m_contentLayer->addChild(
      LevelTile::create(
        id,
        {this->getContentWidth(), 20.f},
        onSelected
      )
    );
  }
  this->scrollLayer->m_contentLayer->updateLayout();
  this->scrollLayer->scrollToTop();
}
