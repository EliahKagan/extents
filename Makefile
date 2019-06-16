CFLAGS += -std=c11 -fsanitize=address,undefined -g

ifeq ($(CC),clang-9)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -pedantic-errors -Wall -Wextra
endif

all: fiemap

.PHONY: clean
clean:
	rm -f fiemap
