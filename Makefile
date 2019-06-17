sanitizers := -fsanitize=address,undefined
CFLAGS += $(sanitizers) -std=c11 -g
LDFLAGS += $(sanitizers)

# FIXME: Check for compilers matching /^clang(-|$)/ -- not just clang-9.
ifeq ($(CC),clang-9)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -pedantic-errors -Wall -Wextra
endif

all: fiemap

fiemap: fiemap.o util.o

fiemap.o: fiemap.c feature-test.h attribute.h util.h

util.o: util.c util.h attribute.h feature-test.h

.PHONY: clean
clean:
	rm -f fiemap *.o
