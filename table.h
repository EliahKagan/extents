// table.h - drawing a custom table of file extents
//
// This file is part of extents, tools for querying and accessing file extents.
//
// Written in 2019 by Eliah Kagan <degeneracypressure@gmail.com>.
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.

#ifndef HAVE_IOCTL_1_FIEMAP_TABLE_H_
#define HAVE_IOCTL_1_FIEMAP_TABLE_H_

#include "feature-test.h"

#include "attribute.h"

#include <stddef.h>
#include <linux/fiemap.h>
#include <linux/types.h>
#include <sys/ioctl.h>

enum datum {
    k_datum_logical,      // "LOGICAL"
    k_datum_physical,     // "INITIAL"
    k_datum_length,       // "COUNT"
    k_datum_physical_end  // "FINAL"
};

struct colspec {
    const char *label;
    int width;
    enum datum datum;
    __u64 offset;
    __u64 divisor;
};

struct tablespec {
    const struct fiemap *fmp;
    int gap_width;
    int col_count;
    struct colspec cols[];
};

ATTRIBUTE((nonnull))
void show_extent_table(const struct fiemap *restrict fmp, const __u64 offset,
                       const char *restrict columns);

#endif // ! HAVE_IOCTL_1_FIEMAP_TABLE_H_
