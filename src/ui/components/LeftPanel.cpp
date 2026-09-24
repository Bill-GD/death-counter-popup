#include "ui/components/LeftPanel.hpp"

#include "handlers/SaveHandler.hpp"
#include "ui/components/LevelTile.hpp"
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
  setID("left-panel"_spr);
  setContentSize({width, listHeight + gap + controlHeight});

  const auto textInput = TextInput::create(width, "Search", "bigFont.fnt");
  textInput->setID("search-input"_spr);
  textInput->setContentSize({width, controlHeight});
  textInput->setAnchorPoint({0.5f, 1.f});
  textInput->setCallback([this](const std::string& value) { onInputChanged(value); });

  m_countLabel = Label::create("bigFont.fnt");
  m_countLabel->setID("level-count-label"_spr);
  m_countLabel->setScale(0.35f);
  m_countLabel->setAnchorPoint({0.5f, 0.5f});

  const auto listRegion = CCScale9Sprite::create("GJ_square05.png");
  listRegion->setID("level-list"_spr);
  listRegion->setContentSize({width, listHeight});
  listRegion->setAnchorPoint({0.5f, 0.f});

  m_loadingSpinner = LoadingSpinner::create(controlHeight);
  m_loadingSpinner->setVisible(false);
  listRegion->addChildAtPosition(m_loadingSpinner, Anchor::Center);

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
  listRegion->addChildAtPosition(scrollbar, Anchor::Right, {-3.f, 0});

  m_scrollLayer = scroll;

  addChildAtPosition(textInput, Anchor::Top);
  addChildAtPosition(m_countLabel, Anchor::Top, {0.f, -(controlHeight + gap / 2.f)});
  addChildAtPosition(listRegion, Anchor::Bottom);
  return true;
}

void LeftPanel::onInputChanged(const std::string& value) {
  m_pendingFilterInput = value;

  stopActionByTag(12345);
  const auto delaySequence = CCSequence::createWithTwoActions(
    CCDelayTime::create(0.3f),
    CCCallFunc::create(this, callfunc_selector(LeftPanel::executeFiltering))
  );
  delaySequence->setTag(12345);
  runAction(delaySequence);
}

void LeftPanel::loadLevelList() {
  m_loadingSpinner->setVisible(true);

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
  m_allLevels = ranges::map<std::vector<std::pair<std::string, LevelInfo>>>(
    allLevelIDs,
    [](auto const& id) {
      return std::pair{id, SaveHandler::getLevelInfo(id)};
    }
  );
  m_filteredLevels = m_allLevels;

  m_loadingSpinner->setVisible(false);
}

void LeftPanel::executeFiltering() {
  if (m_pendingFilterInput.empty()) {
    m_filteredLevels = m_allLevels;
  } else {
    filterLevels(m_pendingFilterInput);
  }
  displayLevelList();
}

void LeftPanel::filterLevels(const std::string& input) {
  m_filteredLevels = ranges::filter(
    m_allLevels,
    [input](auto const& level) {
      return string::toLower(level.first).contains(string::toLower(input))
        || string::toLower(level.second.name).contains(string::toLower(input));
    }
  );
}

void LeftPanel::setOnSelectedCallback(const std::function<void(std::string)>& onSelected) {
  m_onSelectedCallback = onSelected;
}

void LeftPanel::displayLevelList() const {
  m_scrollLayer->m_contentLayer->removeAllChildren();

  for (auto const& [id, level] : m_filteredLevels) {
    m_scrollLayer->m_contentLayer->addChild(
      LevelTile::create(
        LevelTileInfo{id, level.name},
        {getContentWidth(), 30.f},
        m_onSelectedCallback
      )
    );
  }
  m_scrollLayer->m_contentLayer->updateLayout();
  m_scrollLayer->scrollToTop();

  const auto levelCount = m_filteredLevels.size();
  m_countLabel->setString(fmt::format("{} level{}", levelCount, levelCount > 1 ? "s" : "").c_str());
}
