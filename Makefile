sanitizers := -fsanitize=address,undefined
CFLAGS += $(sanitizers) -g -std=c11 -pedantic-errors
LDFLAGS += $(sanitizers)

# FIXME: Check for compilers matching /^clang(-|$)/ -- not just clang-9.
ifeq ($(CC),clang-9)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -Wall -Wextra
endif

srcs = $(wildcard *.c)
objs = $(srcs:.c=.o)
deps = $(srcs:.c=.d)

fiemap: $(objs)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

.PHONY: clean
clean:
	$(RM) fiemap $(objs) $(deps)

-include $(deps)
