# Gomoku

AI bot for the Epitech Gomoku project (pbrain protocol).

## Build

```sh
make
```

Rules available: `make`, `make clean`, `make fclean`, `make re`.

## Run

The binary is named `pbrain-gomoku-ai` and communicates on stdin/stdout.

Quick smoke test:

```sh
printf "START 20\nBEGIN\nEND\n" | ./pbrain-gomoku-ai
```

## Project structure

- `src/`: sources
- `include/`: headers
- `tests/`: tests (optional)
- `bonus/`: bonus files (optional)
