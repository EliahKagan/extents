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
test_input_path := test-symlink
test_output_file := $(target).out

.PHONY: test
test: $(target) $(tester) $(test_input_path)
	./$(target) $(test_input_path) | tee $(test_output_file)
	sudo ./$(tester) $(test_input_path) <$(test_output_file)

.PHONY: check
check: test

.PHONY: clean
clean:
	$(RM) $(target) $(objs) $(deps)

-include $(deps)
