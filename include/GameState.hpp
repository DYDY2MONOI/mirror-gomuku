#pragma once

#include <vector>
#include <utility>
#include <optional>

class GameState {
public:
    using Move = std::pair<int, int>;
    enum class Player {
        None = 0,
        One = 1,
        Two = 2
    };

    explicit GameState(int size);

    int size() const;
    Player currentPlayer() const;
    const std::vector<Move>& history() const;
    // --- C1 API (20x20 board) ---
    // Returns 0 (empty), 1, or 2. Out-of-bounds returns 0.
    int get(int x, int y) const;
    // Sets cell to 0/1/2. Returns false on out-of-bounds/invalid player value.
    bool set(int x, int y, int player);
    bool is_empty(int x, int y) const;
    bool in_bounds(int x, int y) const;

    // Typed helpers used by the bot implementation.
    Player playerAt(int x, int y) const;
    bool isValid(int x, int y) const;
    bool isEmpty(int x, int y) const;

    bool play(int x, int y, Player player);
    void undo();

    void clear();
    void set(int x, int y, Player player);

    // --- C2 Win Detection (five in a row) ---
    // Check if the last move at (x, y) creates a winning line
    bool checkWin(int x, int y) const;
    // Check if a specific player has won anywhere on the board
    bool checkWinFor(Player player) const;
    // Check if there's a winner on the board (returns the winning player or None)
    Player getWinner() const;

private:
    // Helper: count consecutive pieces in a direction (dx, dy)
    int countDirection(int x, int y, int dx, int dy, Player player) const;
    int size_;
    std::vector<Player> board_;
    std::vector<Move> history_;
};
