// table.h - drawing a custom table of file extents

#ifndef HAVE_IOCTL_1_FIEMAP_TABLE_H_
#define HAVE_IOCTL_1_FIEMAP_TABLE_H_

#include "feature-test.h"

#include "attribute.h"

#include <linux/fiemap.h>
#include <sys/ioctl.h>

ATTRIBUTE((nonnull(1)))
void show_extent_table(const struct fiemap *const fmp, const __u64 offset);

#endif // ! HAVE_IOCTL_1_FIEMAP_TABLE_H_
