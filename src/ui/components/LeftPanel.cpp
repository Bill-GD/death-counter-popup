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
  setContentSize({width, listHeight + gap + controlHeight});

  const auto textInput = TextInput::create(width, "Search", "bigFont.fnt");
  textInput->setID("search-input");
  textInput->setContentSize({width, controlHeight});
  textInput->setAnchorPoint({0.5f, 1.f});
  textInput->setCallback([this](const std::string& value) { onInputChanged(value); });

  m_countLabel = Label::create("bigFont.fnt");
  m_countLabel->setID("level-count-label");
  m_countLabel->setScale(0.35f);
  m_countLabel->setAnchorPoint({0.5f, 0.5f});

  const auto listArea = CCScale9Sprite::create("GJ_square05.png");
  listArea->setID("level-list");
  listArea->setContentSize({width, listHeight});
  listArea->setAnchorPoint({0.5f, 0.f});

  m_loadingSpinner = LoadingSpinner::create(controlHeight);
  m_loadingSpinner->setVisible(false);

  const auto scroll = ScrollLayer::create({width - 10.f, listHeight - 4.f});
  const auto scrollSize = scroll->getContentSize();
  scroll->m_contentLayer->setContentSize({scrollSize.width, scrollSize.height - 10.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  const auto scrollbar = Scrollbar::create(scroll);
  scrollbar->setAnchorPoint({1.f, 0.5f});
  scrollbar->setContentSize({4.f, listHeight});

  listArea->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scrollSize / 2.f - CCSize{2.f, 0}
  );
  listArea->addChildAtPosition(scrollbar, Anchor::Right, {-3.f, 0});
  listArea->addChildAtPosition(m_loadingSpinner, Anchor::Center);

  m_scrollLayer = scroll;

  addChildAtPosition(textInput, Anchor::Top);
  addChildAtPosition(m_countLabel, Anchor::Top, {0.f, -(controlHeight + gap / 2.f)});
  addChildAtPosition(listArea, Anchor::Bottom);
  return true;
}

void LeftPanel::onInputChanged(const std::string& value) {
  m_filterInput = value;

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

  std::thread(
    [this] {
      const auto allLevelDirs = FileUtils::getAllDirectories(SaveHandler::PATH);
      const auto allLevelIDs = ranges::filter(
        ranges::map<std::vector<std::string>>(
          allLevelDirs,
          [](auto const& dir) { return dir.filename().string(); }
        ),
        [](auto const& id) { return id != "backups"; }
      );
      auto loadedLevels = ranges::map<std::vector<std::pair<std::string, LevelInfo>>>(
        allLevelIDs,
        [](auto const& id) {
          const auto [idStr, name, link] = SaveHandler::getLevelInfoFromFile(id, false);
          return std::pair{id, LevelInfo{idStr, name}};
        }
      );

      Loader::get()->queueInMainThread(
        [this, data = std::move(loadedLevels)]() mutable {
          m_allLevels = std::move(data);
          m_filteredLevels = m_allLevels;
          m_loadingSpinner->setVisible(false);
          displayLevelList();
          log::info("Loaded {} levels", m_filteredLevels.size());
        }
      );
    }
  ).detach();
}

void LeftPanel::executeFiltering() {
  if (m_filterInput.empty()) {
    m_filteredLevels = m_allLevels;
  } else {
    filterLevels(m_filterInput);
  }
  displayLevelList();
}

void LeftPanel::filterLevels(const std::string& input) {
  log::info("Filtering levels: input={}", m_filterInput);
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
  m_countLabel->setString(fmt::format("{} level{}", levelCount, levelCount == 1 ? "" : "s").c_str());
}
