#pragma once

#include "TimeManager.hpp"
#include <chrono>
#include <cstdint>
#include <memory>
#include <optional>
#include <unordered_map>
#include <utility>
#include <vector>

#include "GameState.hpp"

class Bot {
public:
  using Move = std::pair<int, int>;

  Bot();
  ~Bot();

  bool start(int size);
  bool restart();

  void setRule(int rule);
  void setTimeoutTurnMs(int ms);

  bool applyOpponentMove(Move move);
  bool applyBoardMove(Move move, int player);
  std::optional<Move> chooseMove() const;
  bool applyOurMove(Move move);
  bool takeback(Move move);

  int boardSize() const;
  void setGameState(int size);

private:
  struct TranspositionEntry {
    int depth;
    int score;
  };

  int rule_ = 0;
  std::chrono::milliseconds timeoutTurn_ = std::chrono::seconds(5);

  std::unique_ptr<GameState> gameState_;
  std::unordered_map<std::uint64_t, TranspositionEntry> transpositionTable_;
  int evaluateBoard(const GameState &state, GameState::Player player) const;
  int minimax(int depth, int alpha, int beta, bool maximizingPlayer,
              GameState::Player iaPlayer, TimeManager &timer);
};
