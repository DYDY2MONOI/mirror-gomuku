#pragma once

#include <optional>
#include <utility>
#include <vector>

class GameState {
public:
  using Move = std::pair<int, int>;
  enum class Player { None = 0, One = 1, Two = 2 };

  explicit GameState(int size);

  int size() const;
  Player currentPlayer() const;
  const std::vector<Move> &history() const;
  int get(int x, int y) const;
  bool set(int x, int y, int player);
  bool is_empty(int x, int y) const;
  bool in_bounds(int x, int y) const;

  Player playerAt(int x, int y) const;
  bool isValid(int x, int y) const;
  bool isEmpty(int x, int y) const;

  bool play(int x, int y, Player player);
  void undo();

  void clear();
  void set(int x, int y, Player player);

  bool checkWin(int x, int y) const;
  bool checkWinFor(Player player) const;
  Player getWinner() const;

  bool willWin(int x, int y, Player player) const;

  std::vector<Move> getLegalMoves() const;

private:
  int countDirection(int x, int y, int dx, int dy, Player player) const;
  int size_;
  std::vector<Player> board_;
  std::vector<Move> history_;
};
