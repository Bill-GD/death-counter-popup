#include "ui/data_popup/components/LeftPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "ui/data_popup/components/LevelTile.hpp"
#include "utils/FileUtils.hpp"

LeftPanel* LeftPanel::create(const float width, const float controlHeight, const float listHeight, const float gap) {
  const auto ret = new LeftPanel();
  if (ret->init(width, controlHeight, listHeight, gap)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool LeftPanel::init(float width, float controlHeight, float listHeight, float gap) {
  if (!CCNode::init()) return false;
  setContentSize({width, listHeight + gap + controlHeight});

  const auto textInput = TextInput::create(width, "Search", "bigFont.fnt");
  textInput->setContentSize({width, controlHeight});
  textInput->setAnchorPoint({0.5f, 1.f});
  textInput->setCallback([this](const std::string& value) { onInputChanged(value); });

  countLabel = Label::create("bigFont.fnt");
  countLabel->setScale(0.5f);
  countLabel->setAnchorPoint({0.5f, 0.5f});

  const auto listRegion = CCScale9Sprite::create("GJ_square05.png");
  listRegion->setContentSize({width, listHeight});
  listRegion->setAnchorPoint({0.5f, 0.f});

  const auto scroll = ScrollLayer::create({width - 10.f, listHeight - 4.f});
  const auto scrollSize = scroll->getContentSize();
  scroll->m_contentLayer->setContentSize({scrollSize.width, scrollSize.height - 10.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  const auto scrollbar = Scrollbar::create(scroll);
  scrollbar->setAnchorPoint({1.f, 0.5f});
  scrollbar->setContentSize({4.f, listHeight});

  listRegion->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scrollSize / 2.f - CCSize{2.f, 0}
  );
  listRegion->addChildAtPosition(scrollbar, Anchor::Right, {-4.f, 0});

  this->scrollLayer = scroll;

  addChildAtPosition(textInput, Anchor::Top);
  addChildAtPosition(countLabel, Anchor::Top, {0.f, -(controlHeight + gap / 2.f)});
  addChildAtPosition(listRegion, Anchor::Bottom);
  return true;
}

void LeftPanel::onInputChanged(const std::string& value) {
  pendingFilterInput = value;

  stopActionByTag(12345);
  const auto delaySequence = CCSequence::createWithTwoActions(
    CCDelayTime::create(0.4f),
    CCCallFunc::create(this, callfunc_selector(LeftPanel::executeFiltering))
  );
  delaySequence->setTag(12345);
  runAction(delaySequence);
}

void LeftPanel::loadLevelList() {
  const std::vector<std::filesystem::path> allLevelDirs = FileUtils::getAllDirectories(SaveHandler::PATH);
  const auto allLevelIDs = ranges::filter(
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
  allLevels = ranges::map<std::vector<std::pair<std::string, LevelInfo>>>(
    allLevelIDs,
    [](auto const& id) {
      return std::pair{id, SaveHandler::getLevelInfo(id)};
    }
  );
  filteredLevels = allLevels;
}

void LeftPanel::executeFiltering() {
  if (pendingFilterInput.empty()) {
    filteredLevels = allLevels;
  } else {
    filterLevels(pendingFilterInput);
  }
  displayLevelList();
}

void LeftPanel::filterLevels(const std::string& input) {
  filteredLevels = ranges::filter(
    allLevels,
    [input](auto const& level) {
      return string::toLower(level.first).contains(string::toLower(input))
        || string::toLower(level.second.name).contains(string::toLower(input));
    }
  );
}

void LeftPanel::setOnSelectedCallback(const std::function<void(std::string)>& onSelected) {
  onSelectedCallback = onSelected;
}

void LeftPanel::displayLevelList() const {
  this->scrollLayer->m_contentLayer->removeAllChildren();

  for (auto const& [id, level] : filteredLevels) {
    this->scrollLayer->m_contentLayer->addChild(
      LevelTile::create(
        LevelTileInfo{id, level.name},
        {this->getContentWidth(), 30.f},
        onSelectedCallback
      )
    );
  }
  this->scrollLayer->m_contentLayer->updateLayout();
  this->scrollLayer->scrollToTop();
  countLabel->setString(fmt::format("{} levels", filteredLevels.size()).c_str());
}
