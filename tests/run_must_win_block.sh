#!/bin/bash

# Gomoku Must Win / Must Block Tests
# Usage: ./tests/run_must_win_block.sh

BINARY="./pbrain-gomoku-ai"
TEST_DIR="$(dirname "$0")/must_win_block"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

trim() {
  local value="$1"
  value="${value#"${value%%[![:space:]]*}"}"
  value="${value%"${value##*[![:space:]]}"}"
  printf '%s' "$value"
}

echo -e "${YELLOW}=== Gomoku Must Win / Must Block Tests ===${NC}"

if [ ! -f "$BINARY" ]; then
  echo -e "${RED}Error: Binary not found. Run 'make' first.${NC}"
  exit 1
fi

if [ ! -d "$TEST_DIR" ]; then
  echo -e "${RED}Error: Test directory not found: $TEST_DIR${NC}"
  exit 1
fi

total=0
passed=0
failed=0

for file in "$TEST_DIR"/*.pos; do
  [ -e "$file" ] || continue
  total=$((total + 1))

  size=20
  rule=0
  expect=()
  board_lines=()

  while IFS= read -r raw || [ -n "$raw" ]; do
    line=$(trim "$raw")
    if [ -z "$line" ]; then
      continue
    fi
    if [[ "$line" == \#* ]]; then
      continue
    fi
    case "$line" in
      SIZE\ *)
        size="${line#SIZE }"
        ;;
      RULE\ *)
        rule="${line#RULE }"
        ;;
      EXPECT\ *)
        expect_line="${line#EXPECT }"
        IFS=' ' read -r -a expect <<< "$expect_line"
        ;;
      *)
        board_lines+=("$line")
        ;;
    esac
  done < "$file"

  input="START $size\n"
  if [ "$rule" != "0" ]; then
    input+="INFO rule $rule\n"
  fi
  input+="BOARD\n"
  for line in "${board_lines[@]}"; do
    input+="$line\n"
  done
  input+="DONE\nEND\n"

  output=$(echo -e "$input" | "$BINARY" 2>/dev/null)
  last_line=$(echo "$output" | awk 'NF{line=$0} END{print line}')

  ok=false
  for move in "${expect[@]}"; do
    if [ "$last_line" == "$move" ]; then
      ok=true
      break
    fi
  done

  if [ "$ok" = true ]; then
    passed=$((passed + 1))
    echo -e "${GREEN}✓ PASS${NC}: $(basename "$file") -> $last_line"
  else
    failed=$((failed + 1))
    echo -e "${RED}✗ FAIL${NC}: $(basename "$file")"
    echo "  Expected: ${expect[*]}"
    echo "  Got: $last_line"
  fi

done

echo ""
echo -e "${YELLOW}=== Results ===${NC}"
echo "Passed: $passed"
echo "Failed: $failed"

test "$failed" -eq 0
