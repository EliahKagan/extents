// util.h - utility functions
//
// This file is part of extents, tools for querying and accessing file extents.
//
// Written in 2019 by Eliah Kagan <degeneracypressure@gmail.com>.
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software. If not, see
// <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef HAVE_EXTENTS_FIEMAP_UTIL_H_
#define HAVE_EXTENTS_FIEMAP_UTIL_H_

#include "feature-test.h"

#include "attribute.h"

#include <stddef.h>
#include <stdnoreturn.h>

#define BUG(format) format " (this is a bug!)"

#define ASSERT_NONNEGATIVE_INT_FITS_IN_SIZE_T() \
    static_assert(sizeof(int) <= sizeof(size_t), \
        "I want to convert safely between ints >= 0 and size_ts <= INT_MAX.")

ATTRIBUTE((nonnull))
void set_progname(const char *name);

ATTRIBUTE((returns_nonnull))
const char *progname(void);

ATTRIBUTE((format(printf, 1, 2), nonnull(1)))
noreturn void die(const char *restrict format, ...);

ATTRIBUTE((malloc, returns_nonnull))
void *xcalloc(size_t count, size_t size);

ATTRIBUTE((const))
inline int max(const int first, const int second)
{
    return second > first ? second : first;
}

#endif // ! HAVE_EXTENTS_FIEMAP_UTIL_H_
