#include "Bot.hpp"
#include "GameState.hpp"

#include "GameState.hpp"

Bot::Bot() = default;
Bot::~Bot() = default;

bool Bot::start(int size) {
  if (size < 5 || size > 100)
    return false;

  gameState_ = std::make_unique<GameState>(size);
  myColor_ = 0;
  return true;
}

bool Bot::restart() {
  if (!gameState_)
    return false;
  gameState_->clear();
  myColor_ = 0;
  return true;
}

bool Bot::inBounds(Move move) const {
  return gameState_ && gameState_->isValid(move.first, move.second);
}

bool Bot::isEmpty(Move move) const {
  return gameState_ && gameState_->isEmpty(move.first, move.second);
}

bool Bot::applyOpponentMove(Move move) {
  if (!gameState_)
    return false;

  if (myColor_ == 0) {
    if (gameState_->history().empty()) {
      myColor_ = 2;
    } else {
      myColor_ = 1;
    }
  }

  GameState::Player opponent =
      (myColor_ == 1) ? GameState::Player::Two : GameState::Player::One;
  return gameState_->play(move.first, move.second, opponent);
}

bool Bot::applyOurMove(Move move) {
  if (!gameState_)
    return false;

  if (myColor_ == 0) {
    if (gameState_->history().empty()) {
      myColor_ = 1;
    } else {
      myColor_ = 2;
    }
  }

  GameState::Player us =
      (myColor_ == 1) ? GameState::Player::One : GameState::Player::Two;
  return gameState_->play(move.first, move.second, us);
}

bool Bot::applyBoardMove(Move move, int player) {
  if (!gameState_)
    return false;

  GameState::Player p = GameState::Player::None;
  if (player == 1) {
    p = GameState::Player::One;
    if (myColor_ == 0)
      myColor_ = 1;
    if (myColor_ == 2) {
      p = GameState::Player::One;
    }
  } else if (player == 2) {
    p = GameState::Player::Two;
  } else {
    return false;
  }

  if (myColor_ == 0)
    myColor_ = 1;

  gameState_->set(move.first, move.second, p);
  return true;
}

bool Bot::takeback(Move move) {
  if (!gameState_ || gameState_->history().empty())
    return false;

  if (gameState_->history().back() == move) {
    gameState_->undo();
    return true;
  }
  return false;
}

std::optional<Bot::Move> Bot::chooseMove() const {
  if (!gameState_)
    return std::nullopt;

  int s = gameState_->size();
  Move center{s / 2, s / 2};
  if (gameState_->isEmpty(center.first, center.second))
    return center;

  for (int y = 0; y < s; ++y) {
    for (int x = 0; x < s; ++x) {
      if (gameState_->isEmpty(x, y))
        return Move{x, y};
    }
  }
  return std::nullopt;
}

int Bot::boardSize() const { return gameState_ ? gameState_->size() : 0; }

void Bot::setGameState(int size) { (void)start(size); }
