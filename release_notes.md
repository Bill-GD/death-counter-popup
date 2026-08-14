- Rework how percentage is obtained
  - Timestamp `GJGameLevel::m_timestamp` is total frames/ticks of a level
  - Actual level time info is in `GJGameState`, can be fetched from `GJBaseGameLayer`
    (`m_levelTime` is current time in seconds of player)
  - `GJBaseGameLayer::m_levelLength` is level's spatial length (= endwall position),
    can use with player's x position for calculation
- Fix not saving start percent on 1st attempt