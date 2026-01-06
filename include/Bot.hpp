#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include <chrono>

class GameState;

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
  int rule_ = 0;
  std::chrono::milliseconds timeoutTurn_ = std::chrono::seconds(5);

  std::unique_ptr<GameState> gameState_;
};
