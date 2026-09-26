#include "ui/DCPLevelPopup.hpp"

#include "handlers/SaveHandler.hpp"
#include "ui/components/PrecisionSelector.hpp"

DCPLevelPopup* DCPLevelPopup::create(const std::string& levelID) {
  const auto ret = new DCPLevelPopup();
  if (ret->init(levelID)) {
    ret->autorelease();
    return ret;
  }

  delete ret;
  return nullptr;
}

bool DCPLevelPopup::init(const std::string& levelID) {
  if (!Popup::init(440.f, 280.f)) {
    return false;
  }

  m_levelID = levelID;

  setID("level-data-viewer");
  setTitle("Run Counter (loading...)");

  const auto popupSize = m_mainLayer->getScaledContentSize();
  const auto displayWidth = popupSize.width * 0.8f;
  const auto selectorHeight = popupSize.height * 0.15f;
  const auto dataHeight = popupSize.height * 0.65f;

  const auto filterArea = CCNode::create();
  filterArea->setContentSize({displayWidth, selectorHeight});
  filterArea->setAnchorPoint({0.5f, 1.f});

  const auto precisionInput = CCNode::create();
  precisionInput->setContentSize({displayWidth / 2.4f, selectorHeight});
  precisionInput->setAnchorPoint({0.f, 0.5f});

  const auto precisionInputLabel = CCLabelBMFont::create("Precision", "bigFont.fnt");
  precisionInputLabel->setScale(0.5f);
  precisionInputLabel->setAnchorPoint({0.f, 0.5f});
  precisionInput->addChildAtPosition(precisionInputLabel, Anchor::Left);

  const auto precisionSelector = PrecisionSelector::create(
    {displayWidth * 0.15f, selectorHeight},
    [this](const int value) {
      m_pendingPrecisionFilter = value;

      stopActionByTag(12346);
      const auto delaySequence = CCSequence::createWithTwoActions(
        CCDelayTime::create(0.3f),
        CCCallFunc::create(this, callfunc_selector(DCPLevelPopup::onPrecisionChanged))
      );
      delaySequence->setTag(12346);
      runAction(delaySequence);
    }
  );
  precisionSelector->setAnchorPoint({1.f, 0.5f});
  precisionInput->addChildAtPosition(precisionSelector, Anchor::Right);
  filterArea->addChildAtPosition(precisionInput, Anchor::Left);

  const auto fromZeroInput = CCMenu::create();
  fromZeroInput->setContentSize({displayWidth / 2.5f, selectorHeight});
  fromZeroInput->setAnchorPoint({1.f, 0.5f});

  const auto fromZeroLabel = CCLabelBMFont::create("From Zero", "bigFont.fnt");
  fromZeroLabel->setScale(0.5f);
  fromZeroLabel->setAnchorPoint({1.f, 0.5f});
  fromZeroInput->addChildAtPosition(fromZeroLabel, Anchor::Right);

  const auto fromZeroCheckbox = CCMenuItemToggler::createWithStandardSprites(
    this,
    menu_selector(DCPLevelPopup::onCheckbox),
    1.f
  );
  fromZeroCheckbox->setID("from-zero-toggle");
  fromZeroCheckbox->setAnchorPoint({0.f, 0.5f});
  fromZeroCheckbox->setScale(0.9f);
  fromZeroCheckbox->toggle(true);
  fromZeroInput->addChildAtPosition(fromZeroCheckbox, Anchor::Left);
  filterArea->addChildAtPosition(fromZeroInput, Anchor::Right);

  const auto dataArea = CCScale9Sprite::create("GJ_square05.png");
  dataArea->setID("run-data-display");
  dataArea->setContentSize({displayWidth, dataHeight});
  dataArea->setAnchorPoint({0.5f, 0.f});

  const auto scroll = ScrollLayer::create({displayWidth - 10.f, dataHeight - 4.f});
  const auto scrollSize = scroll->getContentSize();
  scroll->m_contentLayer->setContentSize({scrollSize.width, scrollSize.height - 10.f});
  scroll->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(5.f));

  const auto scrollbar = Scrollbar::create(scroll);
  scrollbar->setAnchorPoint({1.f, 0.5f});
  scrollbar->setContentSize({4.f, dataHeight});

  dataArea->addChildAtPosition(
    scroll,
    Anchor::Center,
    -scrollSize / 2.f - CCSize{2.f, 0}
  );
  dataArea->addChildAtPosition(scrollbar, Anchor::Right, {-3.f, 0});

  const auto infoMenu = CCMenu::create();
  const auto infoButton = InfoAlertButton::create(
    "Level Run Data Viewer",
    R"(Shows run data, can filter by precision and from zero.
For anything other than the run precision, just use Death Tracker, that one is much better, and this'll likely stay this way.)",
    1.f
  );
  infoMenu->addChild(infoButton);
  addChildAtPosition(infoMenu, Anchor::TopRight);

  m_mainLayer->addChildAtPosition(filterArea, Anchor::Top, {0.f, -35.f});
  m_mainLayer->addChildAtPosition(dataArea, Anchor::Bottom, {0.f, 20.f});
  m_scrollLayer = scroll;

  return true;
}

void DCPLevelPopup::load() {
  if (m_levelID.empty()) return;

  for (
    const auto deathCounter = SaveHandler::getSavedData(m_levelID);
    const auto& [key, runData] : deathCounter
  ) {
    m_runData.push_back(
      KeyedRunData{
        .run = key,
        .count = runData.count,
        .precision = runData.precision,
        .parent = runData.parent,
      }
    );
  }

  const auto [_, name, __] = SaveHandler::getLevelInfoFromFile(m_levelID, false);
  setTitle(fmt::format("Run Counter ({})", name));

  filterRuns();
  displayData();
}

void DCPLevelPopup::onCheckbox(CCObject* sender) {
  const auto checkbox = static_cast<CCMenuItemToggler*>(sender);

  const bool wasChecked = checkbox->isToggled();
  m_pendingFromZeroFilter = !wasChecked;
  filterRuns();
  displayData();
}

void DCPLevelPopup::onPrecisionChanged() {
  filterRuns();
  displayData();
}

void DCPLevelPopup::filterRuns() {
  m_filteredRunData = ranges::filter(
    m_runData,
    [this](const auto& el) {
      const auto isFromZero = !el.run.contains('-') || el.run.starts_with('-');
      return el.precision == m_pendingPrecisionFilter && isFromZero == m_pendingFromZeroFilter;
    }
  );
}

void DCPLevelPopup::displayData() {
  m_scrollLayer->m_contentLayer->removeAllChildren();

  for (auto const& runData : m_filteredRunData) {
    const auto label = Label::create(
      fmt::format("{}: {}", runData.run, runData.count),
      "bigFont.fnt"
    );
    label->setContentSize({m_scrollLayer->getContentWidth(), 20.f});
    label->setScale(0.75f);
    m_scrollLayer->m_contentLayer->addChild(label);
  }
  m_scrollLayer->m_contentLayer->updateLayout();
  m_scrollLayer->scrollToTop();
}
