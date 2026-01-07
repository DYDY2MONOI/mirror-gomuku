#include "GameState.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace {
constexpr std::uint64_t kZobristSeed = 0x9e3779b97f4a7c15ULL;
} // namespace

GameState::GameState(int size) : size_(20), board_(20 * 20, Player::None) {
  (void)size;
  initZobrist();
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

std::uint64_t GameState::zobristHash() const { return zobristHash_; }

void GameState::initZobrist() {
  zobristTable_.assign(static_cast<std::size_t>(size_) * size_ * 2, 0);
  std::mt19937_64 rng(kZobristSeed);
  std::uniform_int_distribution<std::uint64_t> dist;
  for (auto &entry : zobristTable_) {
    entry = dist(rng);
  }
  zobristHash_ = 0;
}

void GameState::updateHash(int x, int y, Player oldPlayer, Player newPlayer) {
  if (!isValid(x, y) || oldPlayer == newPlayer) {
    return;
  }
  const std::size_t base =
      static_cast<std::size_t>(y * size_ + x) * 2;
  if (oldPlayer == Player::One) {
    zobristHash_ ^= zobristTable_[base];
  } else if (oldPlayer == Player::Two) {
    zobristHash_ ^= zobristTable_[base + 1];
  }
  if (newPlayer == Player::One) {
    zobristHash_ ^= zobristTable_[base];
  } else if (newPlayer == Player::Two) {
    zobristHash_ ^= zobristTable_[base + 1];
  }
}

bool GameState::play(int x, int y, Player player) {
  if (!isValid(x, y) || !isEmpty(x, y)) {
    return false;
  }

  updateHash(x, y, Player::None, player);
  board_[y * size_ + x] = player;
  history_.emplace_back(x, y);
  return true;
}

void GameState::undo() {
  if (history_.empty()) {
    return;
  }
  Move last = history_.back();
  const int index = last.second * size_ + last.first;
  updateHash(last.first, last.second, board_[index], Player::None);
  board_[index] = Player::None;
  history_.pop_back();
}

void GameState::clear() {
  std::fill(board_.begin(), board_.end(), Player::None);
  history_.clear();
  zobristHash_ = 0;
}

void GameState::set(int x, int y, Player player) {
  if (isValid(x, y)) {
    const int index = y * size_ + x;
    updateHash(x, y, board_[index], player);
    board_[index] = player;
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
  const int index = y * size_ + x;
  auto next = static_cast<Player>(player);
  updateHash(x, y, board_[index], next);
  board_[index] = next;
  return true;
}

bool GameState::is_empty(int x, int y) const { return isEmpty(x, y); }
bool GameState::in_bounds(int x, int y) const { return isValid(x, y); }

int GameState::countDirection(int x, int y, int dx, int dy,
                              Player player) const {
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

  const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (const auto &dir : directions) {
    int dx = dir[0];
    int dy = dir[1];

    int count = 1 + countDirection(x, y, dx, dy, player) +
                countDirection(x, y, -dx, -dy, player);

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

bool GameState::willWin(int x, int y, Player player) const {
  if (!isValid(x, y) || !isEmpty(x, y))
    return false;

  const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};

  for (const auto &dir : directions) {
    int dx = dir[0];
    int dy = dir[1];
    int count = 1 + countDirection(x, y, dx, dy, player) +
                countDirection(x, y, -dx, -dy, player);
    if (count >= 5)
      return true;
  }
  return false;
}

std::vector<GameState::Move> GameState::getLegalMoves() const {
  std::vector<Move> moves;

  bool hasStones = false;
  for (const auto &p : board_) {
    if (p != Player::None) {
      hasStones = true;
      break;
    }
  }

  if (!hasStones) {
    if (size_ > 0) {
      moves.emplace_back(size_ / 2, size_ / 2);
    }
    return moves;
  }

  for (int y = 0; y < size_; ++y) {
    for (int x = 0; x < size_; ++x) {
      if (!isEmpty(x, y))
        continue;

      bool nearStone = false;
      for (int dy = -2; dy <= 2 && !nearStone; ++dy) {
        for (int dx = -2; dx <= 2; ++dx) {
          if (dx == 0 && dy == 0)
            continue;
          int nx = x + dx;
          int ny = y + dy;
          if (isValid(nx, ny) && !isEmpty(nx, ny)) {
            nearStone = true;
            break;
          }
        }
      }
      if (nearStone) {
        moves.emplace_back(x, y);
      }
    }
  }

  if (moves.empty()) {
    for (int y = 0; y < size_; ++y) {
      for (int x = 0; x < size_; ++x) {
        if (isEmpty(x, y)) {
          moves.emplace_back(x, y);
        }
      }
    }
  }

  return moves;
}
