CFLAGS += -ansi -pedantic-errors -Wall -Wextra

all: fiemap

.PHONY: clean
clean:
	rm -f fiemap
