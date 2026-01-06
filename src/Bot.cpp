#include "Bot.hpp"
#include "GameState.hpp"
#include "TimeManager.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <string>
#include <string_view>

Bot::Bot() = default;
Bot::~Bot() = default;

void Bot::setRule(int rule) { rule_ = rule; }

void Bot::setTimeoutTurnMs(int ms) {
  constexpr int maxMs = 5000;
  if (ms <= 0) {
    timeoutTurn_ = std::chrono::milliseconds(maxMs);
    return;
  }
  timeoutTurn_ = std::chrono::milliseconds(std::min(ms, maxMs));
}

static bool isRenjuRule(int rule) { return rule == 2; }

static int countContiguousWithVirtualStone(const GameState &state, int x, int y,
                                           int dx, int dy,
                                           GameState::Player player) {
  int cx = x + dx;
  int cy = y + dy;
  int count = 0;
  while (state.isValid(cx, cy) && state.playerAt(cx, cy) == player) {
    ++count;
    cx += dx;
    cy += dy;
  }
  return count;
}

static int maxLineLengthAfterMove(const GameState &state, int x, int y,
                                  GameState::Player player) {
  const std::array<std::pair<int, int>, 4> dirs = {
      std::make_pair(1, 0), std::make_pair(0, 1), std::make_pair(1, 1),
      std::make_pair(1, -1)};

  int best = 1;
  for (const auto &[dx, dy] : dirs) {
    const int left =
        countContiguousWithVirtualStone(state, x, y, -dx, -dy, player);
    const int right =
        countContiguousWithVirtualStone(state, x, y, dx, dy, player);
    best = std::max(best, 1 + left + right);
  }
  return best;
}

static bool hasFourThreatInDirection(const GameState &state, int x, int y,
                                     int dx, int dy, GameState::Player player) {
  for (int startOffset = -4; startOffset <= 0; ++startOffset) {
    int playerCount = 0;
    int opponentCount = 0;
    int emptyCount = 0;

    for (int i = 0; i < 5; ++i) {
      const int px = x + (startOffset + i) * dx;
      const int py = y + (startOffset + i) * dy;
      if (!state.isValid(px, py)) {
        opponentCount = 1;
        break;
      }

      if (px == x && py == y) {
        ++playerCount;
        continue;
      }

      const auto cell = state.playerAt(px, py);
      if (cell == GameState::Player::None) {
        ++emptyCount;
      } else if (cell == player) {
        ++playerCount;
      } else {
        ++opponentCount;
      }
    }

    if (opponentCount == 0 && playerCount == 4 && emptyCount == 1) {
      return true;
    }
  }
  return false;
}

static bool hasFreeThreeInDirection(const GameState &state, int x, int y,
                                    int dx, int dy, GameState::Player player) {
  constexpr int radius = 5;
  std::string line;
  line.reserve(2 * radius + 1);

  for (int i = -radius; i <= radius; ++i) {
    const int px = x + i * dx;
    const int py = y + i * dy;
    if (!state.isValid(px, py)) {
      line.push_back('#');
      continue;
    }
    if (px == x && py == y) {
      line.push_back('X');
      continue;
    }
    const auto cell = state.playerAt(px, py);
    if (cell == GameState::Player::None) {
      line.push_back('.');
    } else if (cell == player) {
      line.push_back('X');
    } else {
      line.push_back('O');
    }
  }

  const int center = radius;
  constexpr std::array<std::string_view, 5> patterns = {
      ".XXX.", ".XX.X.", ".X.XX.", "..XXX.", ".XXX.."};

  for (const auto pattern : patterns) {
    std::size_t pos = line.find(pattern);
    while (pos != std::string::npos) {
      const int start = static_cast<int>(pos);
      const int end = start + static_cast<int>(pattern.size()) - 1;
      if (start <= center && center <= end) {
        return true;
      }
      pos = line.find(pattern, pos + 1);
    }
  }
  return false;
}

static bool isForbiddenRenjuMove(const GameState &state, int x, int y,
                                 GameState::Player player) {
  if (player != GameState::Player::One) {
    return false;
  }

  const int maxLen = maxLineLengthAfterMove(state, x, y, player);
  if (maxLen > 5) {
    return true;
  }
  if (maxLen == 5) {
    return false;
  }

  const std::array<std::pair<int, int>, 4> dirs = {
      std::make_pair(1, 0), std::make_pair(0, 1), std::make_pair(1, 1),
      std::make_pair(1, -1)};

  int fourThreatDirs = 0;
  int freeThreeDirs = 0;
  for (const auto &[dx, dy] : dirs) {
    if (hasFourThreatInDirection(state, x, y, dx, dy, player)) {
      ++fourThreatDirs;
    }
    if (hasFreeThreeInDirection(state, x, y, dx, dy, player)) {
      ++freeThreeDirs;
    }
  }

  if (fourThreatDirs >= 2) {
    return true;
  }
  if (freeThreeDirs >= 2) {
    return true;
  }
  return false;
}

static bool isLegalMove(const GameState &state, int rule, int x, int y,
                        GameState::Player player) {
  if (!state.isValid(x, y) || !state.isEmpty(x, y)) {
    return false;
  }
  if (isRenjuRule(rule) && isForbiddenRenjuMove(state, x, y, player)) {
    return false;
  }
  return true;
}

bool Bot::start(int size) {
  if (size < 5 || size > 100)
    return false;

  gameState_ = std::make_unique<GameState>(size);
  return true;
}

bool Bot::restart() {
  if (!gameState_)
    return false;
  gameState_->clear();
  return true;
}

bool Bot::applyOpponentMove(Move move) {
  if (!gameState_)
    return false;

  const auto opponent = gameState_->currentPlayer();
  if (!isLegalMove(*gameState_, rule_, move.first, move.second, opponent)) {
    return false;
  }
  return gameState_->play(move.first, move.second, opponent);
}

bool Bot::applyOurMove(Move move) {
  if (!gameState_)
    return false;

  const auto us = gameState_->currentPlayer();
  if (!isLegalMove(*gameState_, rule_, move.first, move.second, us)) {
    return false;
  }
  return gameState_->play(move.first, move.second, us);
}

bool Bot::applyBoardMove(Move move, int player) {
  if (!gameState_)
    return false;

  if (player != 1 && player != 2) {
    return false;
  }

  if (!gameState_->in_bounds(move.first, move.second)) {
    return false;
  }
  const int existing = gameState_->get(move.first, move.second);
  if (existing != 0 && existing != player) {
    return false;
  }

  GameState::Player p =
      (player == 1) ? GameState::Player::One : GameState::Player::Two;
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

  TimeManager timer;
  auto budget = timeoutTurn_;
  if (budget > std::chrono::milliseconds(50)) {
    budget -= std::chrono::milliseconds(50);
  }
  timer.start(budget);

  auto moves = gameState_->getLegalMoves();
  if (moves.empty())
    return std::nullopt;

  GameState::Player us = gameState_->currentPlayer();
  GameState::Player opponent = (us == GameState::Player::One)
                                   ? GameState::Player::Two
                                   : GameState::Player::One;

  std::optional<Move> blockingMove;
  std::optional<Move> fallbackMove;

  for (const auto &move : moves) {
    if (timer.expired()) {
      break;
    }
    if (!isLegalMove(*gameState_, rule_, move.first, move.second, us)) {
      continue;
    }

    if (!fallbackMove) {
      fallbackMove = move;
    }

    if (gameState_->willWin(move.first, move.second, us)) {
      return move;
    }

    if (gameState_->willWin(move.first, move.second, opponent)) {
      if (!blockingMove) {
        blockingMove = move;
      }
    }
  }

  if (blockingMove) {
    return blockingMove;
  }

  return fallbackMove;
}

int Bot::boardSize() const { return gameState_ ? gameState_->size() : 0; }

void Bot::setGameState(int size) { (void)start(size); }
