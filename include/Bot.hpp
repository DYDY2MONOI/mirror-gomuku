#pragma once

#include <memory>
#include <optional>
#include <utility>
#include <vector>

class GameState;

class Bot {
public:
  using Move = std::pair<int, int>;

  Bot();
  ~Bot();

  bool start(int size);
  bool restart();

  bool applyOpponentMove(Move move);
  bool applyBoardMove(Move move, int player);
  std::optional<Move> chooseMove() const;
  bool applyOurMove(Move move);
  bool takeback(Move move);

  int boardSize() const;
  void setGameState(int size);

private:
  bool inBounds(Move move) const;
  bool isEmpty(Move move) const;

  std::unique_ptr<GameState> gameState_;
  int myColor_ = 0;
};
