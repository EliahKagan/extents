sanitizers := -fsanitize=address,undefined
override CFLAGS += $(sanitizers) -g -std=c11 -pedantic-errors
override LDFLAGS += $(sanitizers)

ifeq ($(shell ./is-clang $(CC)),yes)
	CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	CFLAGS += -Wall -Wextra
endif

mapper := fiemap
srcs := $(wildcard *.c)
objs := $(srcs:.c=.o)
deps := $(srcs:.c=.d)

$(mapper): $(objs)

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

stitcher := stitch
test_file := test-symlink
test_log := $(mapper).out
stitched_file := $(test_file).stitched

.PHONY test
test: $(mapper) $(stitcher) $(test_file)
	./$(mapper) $(test_file) | tee $(test_log)
	sudo ./$(stitcher) <$(test_log) >$(stitched_file)
	cmp $(test_file) $(stitched_file)

.PHONY: check
check: test

.PHONY: clean
clean:
	$(RM) $(mapper) $(objs) $(deps)

-include $(deps)
