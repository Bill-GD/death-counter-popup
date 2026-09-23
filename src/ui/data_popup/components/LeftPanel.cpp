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

  const auto controlRegion = CCMenu::create();
  controlRegion->setContentSize({width, controlHeight});
  controlRegion->setAnchorPoint({0.5f, 1.f});

  const auto textInput = TextInput::create(width, "Search", "bigFont.fnt");
  textInput->setAnchorPoint({0.f, 0.5f});
  textInput->setCallback([this](const std::string& value) { onInputChanged(value); });

  controlRegion->addChildAtPosition(textInput, Anchor::Left);

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

  addChildAtPosition(controlRegion, Anchor::Top);
  addChildAtPosition(countLabel, Anchor::Top, {0.f, -(controlHeight + gap / 2.f)});
  addChildAtPosition(listRegion, Anchor::Bottom);
  return true;
}

void LeftPanel::onInputChanged(const std::string& value) {
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
  countLabel->setString(fmt::format("{} levels", filteredLevelIDs.size()).c_str());
}
