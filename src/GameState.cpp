#include "GameState.hpp"
#include <algorithm>

GameState::GameState(int size)
    : size_(20), board_(20 * 20, Player::None) {
  (void)size;
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

GameState::Player GameState::playerAt(int x, int y) const {
  if (!isValid(x, y)) {
    return Player::None;
  }
  return board_[y * size_ + x];
}

bool GameState::isValid(int x, int y) const {
  return x >= 0 && y >= 0 && x < size_ && y < size_;
}

bool GameState::isEmpty(int x, int y) const {
  return playerAt(x, y) == Player::None;
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

int GameState::get(int x, int y) const {
  return static_cast<int>(playerAt(x, y));
}

bool GameState::set(int x, int y, int player) {
  if (!isValid(x, y)) {
    return false;
  }
  if (player < 0 || player > 2) {
    return false;
  }
  board_[y * size_ + x] = static_cast<Player>(player);
  return true;
}

bool GameState::is_empty(int x, int y) const { return isEmpty(x, y); }
bool GameState::in_bounds(int x, int y) const { return isValid(x, y); }

// --- C2 Win Detection Implementation ---

int GameState::countDirection(int x, int y, int dx, int dy, Player player) const {
  int count = 0;
  int nx = x + dx;
  int ny = y + dy;
  
  while (isValid(nx, ny) && playerAt(nx, ny) == player) {
    count++;
    nx += dx;
    ny += dy;
  }
  return count;
}

bool GameState::checkWin(int x, int y) const {
  Player player = playerAt(x, y);
  if (player == Player::None) {
    return false;
  }

  // Check all four directions: horizontal, vertical, and two diagonals
  // For each direction, count consecutive pieces in both ways
  
  // Direction pairs: (dx, dy) and (-dx, -dy)
  const int directions[4][2] = {
    {1, 0},   // Horizontal (left-right)
    {0, 1},   // Vertical (up-down)
    {1, 1},   // Diagonal (top-left to bottom-right)
    {1, -1}   // Anti-diagonal (bottom-left to top-right)
  };

  for (const auto& dir : directions) {
    int dx = dir[0];
    int dy = dir[1];
    
    // Count in positive direction and negative direction, plus the current piece
    int count = 1 + countDirection(x, y, dx, dy, player) 
                  + countDirection(x, y, -dx, -dy, player);
    
    if (count >= 5) {
      return true;
    }
  }
  
  return false;
}

bool GameState::checkWinFor(Player player) const {
  if (player == Player::None) {
    return false;
  }
  
  for (int y = 0; y < size_; ++y) {
    for (int x = 0; x < size_; ++x) {
      if (playerAt(x, y) == player && checkWin(x, y)) {
        return true;
      }
    }
  }
  return false;
}

GameState::Player GameState::getWinner() const {
  if (checkWinFor(Player::One)) {
    return Player::One;
  }
  if (checkWinFor(Player::Two)) {
    return Player::Two;
  }
  return Player::None;
}
