// table.h - drawing a custom table of file extents

#ifndef HAVE_IOCTL_1_FIEMAP_TABLE_H_
#define HAVE_IOCTL_1_FIEMAP_TABLE_H_

#include "feature-test.h"

#include "attribute.h"

#include <stddef.h>
#include <linux/fiemap.h>
#include <linux/types.h>
#include <sys/ioctl.h>

enum field { field_logical, field_physical, field_length };

struct colspec {
    const char *label;
    int width;
    enum field field;
    __u64 offset;
    __u64 divisor;
};

struct tablespec {
    const struct fiemap *fmp;
    int gap_width;
    int col_count;
    struct colspec cols[];
};

ATTRIBUTE((malloc, returns_nonnull))
struct tablespec *alloc_tablespec(int col_count);

ATTRIBUTE((nonnull))
void populate_widths(struct tablespec *tsp);

ATTRIBUTE((nonnull))
void show_extent_table(const struct tablespec *tsp);

#endif // ! HAVE_IOCTL_1_FIEMAP_TABLE_H_
