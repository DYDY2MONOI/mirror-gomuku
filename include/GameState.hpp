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
    Player get(int x, int y) const;
    bool isValid(int x, int y) const;
    bool isEmpty(int x, int y) const;

    bool play(int x, int y, Player player);
    void undo();

    void clear();
    void set(int x, int y, Player player);

private:
    int size_;
    std::vector<Player> board_;
    std::vector<Move> history_;
};
