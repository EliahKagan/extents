CFLAGS += -std=c11 -fsanitize=address,undefined -g
LDFLAGS += -fsanitize=address,undefined

ifeq ($(CC),clang-9)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -pedantic-errors -Wall -Wextra
endif

all: fiemap

fiemap.o: fiemap.c feature-test.h

#fiemap: feature-test.h

.PHONY: clean
clean:
	rm -f fiemap fiemap.o
