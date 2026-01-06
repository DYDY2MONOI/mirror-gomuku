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

echo -e "${YELLOW}=== Tests Complete ===${NC}"
