// util.h - interface to utility functions

#ifndef HAVE_IOCTL_1_FIEMAP_UTIL_H_
#define HAVE_IOCTL_1_FIEMAP_UTIL_H_

#include "attribute.h"
#include "feature-test.h"

#include <stddef.h>
#include <stdnoreturn.h>

void set_progname(const char *name);

const char *progname(void);

ATTRIBUTE((format(printf, 1, 2)))
noreturn void die(const char *restrict format, ...);

// TODO: add a GNU attribute to this to improve performance
void *xcalloc(size_t count, size_t size);

#endif // ! HAVE_IOCTL_1_FIEMAP_UTIL_H_
