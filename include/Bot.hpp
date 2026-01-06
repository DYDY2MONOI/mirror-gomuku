#pragma once

#include <optional>
#include <utility>
#include <vector>

class Bot {
public:
    using Move = std::pair<int, int>;

    bool start(int size);
    bool restart();

    bool applyOpponentMove(Move move);
    bool applyBoardMove(Move move, int player);
    std::optional<Move> chooseMove() const;
    bool applyOurMove(Move move);
    bool takeback(Move move);

    int size() const;

private:
    bool inBounds(Move move) const;
    bool isEmpty(Move move) const;
    int &cell(Move move);
    const int &cell(Move move) const;

    int boardSize_ = 0;
    std::vector<int> board_;
};
