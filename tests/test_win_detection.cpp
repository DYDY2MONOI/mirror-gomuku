/**
 * Win Detection Tests for Gomoku (C2 - Five in a row)
 * 
 * This test file verifies the checkWin functionality that detects
 * five consecutive pieces in rows, columns, and diagonals.
 */

#include "../include/GameState.hpp"
#include <iostream>
#include <string>
#include <cassert>

// Test result counters
int passed = 0;
int failed = 0;

void reportTest(const std::string& name, bool success) {
    if (success) {
        std::cout << "\033[32m✓ PASS\033[0m: " << name << std::endl;
        passed++;
    } else {
        std::cout << "\033[31m✗ FAIL\033[0m: " << name << std::endl;
        failed++;
    }
}

// Test 1: Horizontal win
void testHorizontalWin() {
    GameState game(20);
    
    // Place 5 consecutive pieces horizontally for Player 1
    for (int x = 5; x < 10; ++x) {
        game.set(x, 10, GameState::Player::One);
    }
    
    // Check win at each position in the row
    bool allWin = true;
    for (int x = 5; x < 10; ++x) {
        if (!game.checkWin(x, 10)) {
            allWin = false;
            break;
        }
    }
    
    reportTest("Horizontal win (5 pieces at y=10, x=5-9)", allWin);
}

// Test 2: Vertical win
void testVerticalWin() {
    GameState game(20);
    
    // Place 5 consecutive pieces vertically for Player 2
    for (int y = 3; y < 8; ++y) {
        game.set(7, y, GameState::Player::Two);
    }
    
    // Check win at middle position
    bool win = game.checkWin(7, 5);
    reportTest("Vertical win (5 pieces at x=7, y=3-7)", win);
}

// Test 3: Diagonal win (top-left to bottom-right)
void testDiagonalWin() {
    GameState game(20);
    
    // Place 5 consecutive pieces diagonally
    for (int i = 0; i < 5; ++i) {
        game.set(i, i, GameState::Player::One);
    }
    
    // Check win at center of diagonal
    bool win = game.checkWin(2, 2);
    reportTest("Diagonal win (top-left to bottom-right)", win);
}

// Test 4: Anti-diagonal win (bottom-left to top-right)
void testAntiDiagonalWin() {
    GameState game(20);
    
    // Place 5 consecutive pieces on anti-diagonal
    for (int i = 0; i < 5; ++i) {
        game.set(i, 10 - i, GameState::Player::Two);
    }
    
    // Check win at middle position
    bool win = game.checkWin(2, 8);
    reportTest("Anti-diagonal win (bottom-left to top-right)", win);
}

// Test 5: No win with 4 pieces
void testNoWinFourPieces() {
    GameState game(20);
    
    // Place only 4 consecutive pieces
    for (int x = 0; x < 4; ++x) {
        game.set(x, 5, GameState::Player::One);
    }
    
    bool noWin = !game.checkWin(2, 5);
    reportTest("No win with only 4 consecutive pieces", noWin);
}

// Test 6: Win after adding fifth piece
void testWinAfterFifthPiece() {
    GameState game(20);
    
    // Place 4 pieces first
    for (int x = 0; x < 4; ++x) {
        game.set(x, 5, GameState::Player::One);
    }
    
    bool noWinBefore = !game.checkWin(2, 5);
    
    // Add the 5th piece
    game.set(4, 5, GameState::Player::One);
    bool winAfter = game.checkWin(4, 5);
    
    reportTest("Win detected after adding 5th piece", noWinBefore && winAfter);
}

// Test 7: Win at board edge (left edge)
void testWinAtLeftEdge() {
    GameState game(20);
    
    // Horizontal win starting at left edge
    for (int x = 0; x < 5; ++x) {
        game.set(x, 10, GameState::Player::One);
    }
    
    bool win = game.checkWin(0, 10);
    reportTest("Win at left board edge", win);
}

// Test 8: Win at board edge (right edge)
void testWinAtRightEdge() {
    GameState game(20);
    
    // Horizontal win ending at right edge
    for (int x = 15; x < 20; ++x) {
        game.set(x, 10, GameState::Player::One);
    }
    
    bool win = game.checkWin(19, 10);
    reportTest("Win at right board edge", win);
}

// Test 9: Win at corner
void testWinAtCorner() {
    GameState game(20);
    
    // Diagonal win in corner
    for (int i = 0; i < 5; ++i) {
        game.set(15 + i, 15 + i, GameState::Player::Two);
    }
    
    bool win = game.checkWin(19, 19);
    reportTest("Win at board corner (diagonal)", win);
}

// Test 10: More than 5 in a row
void testMoreThanFiveInRow() {
    GameState game(20);
    
    // Place 7 consecutive pieces
    for (int x = 2; x < 9; ++x) {
        game.set(x, 8, GameState::Player::One);
    }
    
    bool win = game.checkWin(5, 8);
    reportTest("Win detected with more than 5 in a row (7 pieces)", win);
}

// Test 11: getWinner() returns correct player
void testGetWinner() {
    GameState game(20);
    
    // No winner initially
    bool noWinnerInitially = (game.getWinner() == GameState::Player::None);
    
    // Create a win for Player One
    for (int x = 0; x < 5; ++x) {
        game.set(x, 0, GameState::Player::One);
    }
    
    bool playerOneWins = (game.getWinner() == GameState::Player::One);
    
    reportTest("getWinner() returns correct winning player", noWinnerInitially && playerOneWins);
}

// Test 12: checkWinFor() specific player
void testCheckWinFor() {
    GameState game(20);
    
    // Create a win for Player Two
    for (int y = 5; y < 10; ++y) {
        game.set(3, y, GameState::Player::Two);
    }
    
    bool playerTwoWins = game.checkWinFor(GameState::Player::Two);
    bool playerOneNoWin = !game.checkWinFor(GameState::Player::One);
    
    reportTest("checkWinFor() correctly identifies winning player", playerTwoWins && playerOneNoWin);
}

// Test 13: Broken line (gap in middle) - should NOT win
void testBrokenLineNoWin() {
    GameState game(20);
    
    // Place pieces with a gap: X X X _ X X
    game.set(0, 5, GameState::Player::One);
    game.set(1, 5, GameState::Player::One);
    game.set(2, 5, GameState::Player::One);
    // Gap at (3, 5)
    game.set(4, 5, GameState::Player::One);
    game.set(5, 5, GameState::Player::One);
    
    bool noWin = !game.checkWin(2, 5);
    reportTest("Broken line with gap does NOT win", noWin);
}

// Test 14: Mixed players - should NOT count as win
void testMixedPlayersNoWin() {
    GameState game(20);
    
    // Place alternating pieces
    game.set(0, 5, GameState::Player::One);
    game.set(1, 5, GameState::Player::Two);  // Different player!
    game.set(2, 5, GameState::Player::One);
    game.set(3, 5, GameState::Player::One);
    game.set(4, 5, GameState::Player::One);
    
    bool noWin = !game.checkWin(2, 5);
    reportTest("Mixed player pieces do NOT count as win", noWin);
}

// Test 15: Empty cell check
void testEmptyCellNoWin() {
    GameState game(20);
    
    bool noWin = !game.checkWin(10, 10);
    reportTest("Empty cell correctly returns no win", noWin);
}

int main() {
    std::cout << "\033[33m=== Gomoku Win Detection Tests (C2) ===\033[0m\n" << std::endl;
    
    testHorizontalWin();
    testVerticalWin();
    testDiagonalWin();
    testAntiDiagonalWin();
    testNoWinFourPieces();
    testWinAfterFifthPiece();
    testWinAtLeftEdge();
    testWinAtRightEdge();
    testWinAtCorner();
    testMoreThanFiveInRow();
    testGetWinner();
    testCheckWinFor();
    testBrokenLineNoWin();
    testMixedPlayersNoWin();
    testEmptyCellNoWin();
    
    std::cout << "\n\033[33m=== Results ===\033[0m" << std::endl;
    std::cout << "Passed: " << passed << std::endl;
    std::cout << "Failed: " << failed << std::endl;
    
    if (failed == 0) {
        std::cout << "\n\033[32m✓ All tests passed!\033[0m" << std::endl;
        return 0;
    } else {
        std::cout << "\n\033[31m✗ Some tests failed!\033[0m" << std::endl;
        return 1;
    }
}
