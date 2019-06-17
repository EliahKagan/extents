// util.h - interface to utility functions

#ifndef HAVE_IOCTL_1_FIEMAP_UTIL_H_
#define HAVE_IOCTL_1_FIEMAP_UTIL_H_

#include "feature-test.h"

#include <stddef.h>
#include <stdnoreturn.h>

void set_progname(const char *name);

const char *progname(void);

noreturn void die(const char *restrict format, ...);

void *xcalloc(size_t count, size_t size);

#endif // ! HAVE_IOCTL_1_FIEMAP_UTIL_H_
