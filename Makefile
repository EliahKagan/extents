# Makefile - builds fiemap, faciltiates testing fiemap and stitch
#
# This file is part of extents, tools for querying and accessing file extents.
#
# Written in 2019 by Eliah Kagan <degeneracypressure@gmail.com>.
#
# To the extent possible under law, the author(s) have dedicated all copyright
# and related and neighboring rights to this software to the public domain
# worldwide. This software is distributed without any warranty.

sanitizers := -fsanitize=address,undefined
override CFLAGS += $(sanitizers) -g -std=c11 -pedantic-errors
override LDFLAGS += $(sanitizers)

ifeq ($(shell ./is-clang $(CC)),yes)
	override CFLAGS += -Weverything -Wno-disabled-macro-expansion
else
	override CFLAGS += -Wall -Wextra
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

.PHONY: test
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
