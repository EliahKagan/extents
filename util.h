// util.h - interface to utility functions

#ifndef HAVE_IOCTL_1_FIEMAP_UTIL_H_
#define HAVE_IOCTL_1_FIEMAP_UTIL_H_

#include "feature-test.h"

#include "attribute.h"

#include <stddef.h>
#include <stdnoreturn.h>

void set_progname(const char *name);

const char *progname(void);

ATTRIBUTE((format(printf, 1, 2)))
noreturn void die(const char *restrict format, ...);

ATTRIBUTE((malloc, returns_nonnull))
void *xcalloc(size_t count, size_t size);

#endif // ! HAVE_IOCTL_1_FIEMAP_UTIL_H_
