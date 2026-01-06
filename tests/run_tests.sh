#!/bin/bash

# Gomoku Bot Response Format Tests
# Usage: ./tests/run_tests.sh

BINARY="./pbrain-gomoku-ai"
TEST_DIR="$(dirname "$0")"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${YELLOW}=== Gomoku Bot Response Format Tests ===${NC}"
echo ""

# Check if binary exists
if [ ! -f "$BINARY" ]; then
    echo -e "${RED}Error: Binary not found. Run 'make' first.${NC}"
    exit 1
fi

# Test 1: Basic commands
echo -e "${YELLOW}Test 1: Running basic command test${NC}"
echo "Input:"
cat "$TEST_DIR/test_commands.txt"
echo ""
echo "Output:"
"$BINARY" < "$TEST_DIR/test_commands.txt"
echo ""

# Test 2: Individual command tests with expected output validation
echo -e "${YELLOW}Test 2: Individual response format validation${NC}"

# Helper function to test a command
test_command() {
    local input="$1"
    local expected_pattern="$2"
    local description="$3"
    
    output=$(echo -e "$input" | "$BINARY" 2>/dev/null)
    
    if echo "$output" | grep -qE "$expected_pattern"; then
        echo -e "${GREEN}✓ PASS${NC}: $description"
        echo "  Input: $input"
        echo "  Output: $output"
    else
        echo -e "${RED}✗ FAIL${NC}: $description"
        echo "  Input: $input"
        echo "  Expected pattern: $expected_pattern"
        echo "  Got: $output"
    fi
    echo ""
}

# Test START command -> OK
test_command "START 20\nEND" "^OK$" "START command returns OK"

# Test START with invalid size -> ERROR
test_command "START 3\nEND" "^ERROR" "START with invalid size returns ERROR"

# Test BEGIN command -> X,Y format (coordinates)
test_command "START 20\nBEGIN\nEND" "^[0-9]+,[0-9]+$" "BEGIN returns coordinates in X,Y format"

# Test TURN command -> X,Y format
test_command "START 20\nTURN 5,5\nEND" "^[0-9]+,[0-9]+$" "TURN returns coordinates in X,Y format"

# Test ABOUT command -> name="...", version="...", etc.
test_command "ABOUT\nEND" 'name=".*", version=".*", author=".*", country=".*"' "ABOUT returns formatted metadata"

# Test UNKNOWN command
test_command "FOOBAR\nEND" "^UNKNOWN$" "Unknown command returns UNKNOWN"

# Test RESTART without START -> ERROR
test_command "RESTART\nEND" "^ERROR$" "RESTART without START returns ERROR"

# Test TAKEBACK
test_command "START 20\nBEGIN\nTAKEBACK 10,10\nEND" "^OK$" "TAKEBACK returns OK"

# Test forbidden move avoidance (Renju / overline)
test_forbidden_avoid() {
    local input="$1"
    local forbidden_move="$2"
    local description="$3"

    last_line=$(echo -e "$input" | "$BINARY" 2>/dev/null | tail -n 1)

    if [[ "$last_line" == "$forbidden_move" ]]; then
        echo -e "${RED}✗ FAIL${NC}: $description"
        echo "  Forbidden move played: $last_line"
    elif echo "$last_line" | grep -qE "^[0-9]+,[0-9]+$"; then
        echo -e "${GREEN}✓ PASS${NC}: $description"
        echo "  Output: $last_line"
    else
        echo -e "${RED}✗ FAIL${NC}: $description"
        echo "  Expected a move (X,Y) not equal to $forbidden_move"
        echo "  Got: $last_line"
    fi
    echo ""
}

test_forbidden_avoid "START 20\nINFO rule 2\nBOARD\n0,0,1\n1,0,1\n2,0,1\n3,0,1\n4,0,1\n10,10,2\n11,10,2\n12,10,2\n13,10,2\n14,10,2\nDONE\nEND" "5,0" "Renju: bot avoids overline-forbidden move"

# Test 5 seconds max per move (best-effort via `timeout`)
if command -v timeout >/dev/null 2>&1; then
    if timeout 5 bash -c "echo -e 'START 20\nBEGIN\nEND' | \"$BINARY\" >/dev/null 2>/dev/null"; then
        echo -e "${GREEN}✓ PASS${NC}: Move reply under 5 seconds"
    else
        echo -e "${RED}✗ FAIL${NC}: Move reply exceeded 5 seconds"
    fi
    echo ""
fi

echo -e "${YELLOW}=== Tests Complete ===${NC}"
