sanitizers := -fsanitize=address,undefined
override CFLAGS += $(sanitizers) -g -std=c11 -pedantic-errors
override LDFLAGS += $(sanitizers)

ifeq ($(shell ./is-clang $(CC)),yes)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -Wall -Wextra
endif

target := fiemap
srcs := $(wildcard *.c)
objs := $(srcs:.c=.o)
deps := $(srcs:.c=.d)

$(target): $(objs)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

tester := $(target)-test
test_input := test-symlink
test_output := $(target).out
test_elevator := sudo

.PHONY: test
test: $(target) $(tester) $(test_input)
	./$(target) $(test_input) >$(test_output)
	$(test_elevator) ./$(tester) $(test_input) <$(test_output)

.PHONY: check
check: test

.PHONY: clean
clean:
	$(RM) $(target) $(objs) $(deps)

-include $(deps)
