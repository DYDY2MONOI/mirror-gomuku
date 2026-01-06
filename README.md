# Gomoku

AI bot for the Epitech Gomoku project (pbrain protocol).

## Build

```sh
make
```

Rules available: `make`, `make clean`, `make fclean`, `make re`.

Debug build:

```sh
make debug
```

## Run

The binary is named `pbrain-gomoku-ai` and communicates on stdin/stdout.

Quick smoke test:

```sh
printf "START 20\nBEGIN\nEND\n" | ./pbrain-gomoku-ai
```

## Debug / logs

Never print debug information on stdout (it would break the pbrain protocol). This project logs to stderr or to a file.

- Enable stderr logging: `./pbrain-gomoku-ai --debug` (or `GOMOKU_DEBUG=1`)
- Log to a file: `./pbrain-gomoku-ai --log gomoku_debug.log` (or `GOMOKU_LOG=gomoku_debug.log`)

## Project structure

- `src/`: sources
- `include/`: headers
- `tests/`: tests (optional)
- `bonus/`: bonus files (optional)
