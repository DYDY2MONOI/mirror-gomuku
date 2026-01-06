#include "GameState.hpp"
#include <algorithm>

GameState::GameState(int size)
    : size_(size), board_(size * size, Player::None) {
  if (size <= 0) {
    size_ = 20;
    board_.resize(size_ * size_, Player::None);
  }
}

int GameState::size() const { return size_; }

GameState::Player GameState::currentPlayer() const {
  if (!history_.empty()) {
    return (history_.size() % 2 == 0) ? Player::One : Player::Two;
  }
  long p1Count = std::count(board_.begin(), board_.end(), Player::One);
  long p2Count = std::count(board_.begin(), board_.end(), Player::Two);

  if (p1Count == p2Count) {
    return Player::One;
  }
  return Player::Two;
}

const std::vector<GameState::Move> &GameState::history() const {
  return history_;
}

GameState::Player GameState::get(int x, int y) const {
  if (!isValid(x, y)) {
    return Player::None;
  }
  return board_[y * size_ + x];
}

bool GameState::isValid(int x, int y) const {
  return x >= 0 && y >= 0 && x < size_ && y < size_;
}

bool GameState::isEmpty(int x, int y) const {
  return get(x, y) == Player::None;
}

bool GameState::play(int x, int y, Player player) {
  if (!isValid(x, y) || !isEmpty(x, y)) {
    return false;
  }

  board_[y * size_ + x] = player;
  history_.emplace_back(x, y);
  return true;
}

void GameState::undo() {
  if (history_.empty()) {
    return;
  }
  Move last = history_.back();
  board_[last.second * size_ + last.first] = Player::None;
  history_.pop_back();
}

void GameState::clear() {
  std::fill(board_.begin(), board_.end(), Player::None);
  history_.clear();
}

void GameState::set(int x, int y, Player player) {
  if (isValid(x, y)) {
    board_[y * size_ + x] = player;
  }
}
