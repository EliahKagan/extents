CFLAGS += -std=c11 -pedantic-errors -Wall -Wextra -fsanitize=address,undefined -g -Og

all: fiemap

.PHONY: clean
clean:
	rm -f fiemap
