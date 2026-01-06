#include "Bot.hpp"

bool Bot::start(int size)
{
    if (size < 5 || size > 100)
        return false;
    boardSize_ = size;
    board_.assign(static_cast<size_t>(boardSize_ * boardSize_), 0);
    return true;
}

bool Bot::restart()
{
    if (boardSize_ <= 0)
        return false;
    std::fill(board_.begin(), board_.end(), 0);
    return true;
}

bool Bot::inBounds(Move move) const
{
    return move.first >= 0 && move.second >= 0 && move.first < boardSize_ && move.second < boardSize_;
}

int &Bot::cell(Move move)
{
    return board_.at(static_cast<size_t>(move.second * boardSize_ + move.first));
}

const int &Bot::cell(Move move) const
{
    return board_.at(static_cast<size_t>(move.second * boardSize_ + move.first));
}

bool Bot::isEmpty(Move move) const
{
    return cell(move) == 0;
}

bool Bot::applyOpponentMove(Move move)
{
    if (!inBounds(move) || !isEmpty(move))
        return false;
    cell(move) = 2;
    return true;
}

bool Bot::applyOurMove(Move move)
{
    if (!inBounds(move) || !isEmpty(move))
        return false;
    cell(move) = 1;
    return true;
}

bool Bot::applyBoardMove(Move move, int player)
{
    if (!inBounds(move))
        return false;
    if (player != 0 && !isEmpty(move))
        return false;
    if (player < 0 || player > 2)
        return false;
    cell(move) = player;
    return true;
}

bool Bot::takeback(Move move)
{
    if (!inBounds(move) || isEmpty(move))
        return false;
    cell(move) = 0;
    return true;
}

std::optional<Bot::Move> Bot::chooseMove() const
{
    if (boardSize_ <= 0 || board_.empty())
        return std::nullopt;

    const Move center{boardSize_ / 2, boardSize_ / 2};
    if (inBounds(center) && isEmpty(center))
        return center;

    for (int y = 0; y < boardSize_; ++y) {
        for (int x = 0; x < boardSize_; ++x) {
            const Move candidate{x, y};
            if (isEmpty(candidate))
                return candidate;
        }
    }
    return std::nullopt;
}

int Bot::size() const
{
    return boardSize_;
}
