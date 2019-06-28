// filesystem.h - general filesystem access functions (not specific to extents)
// FIXME: Maybe these functions really should just be in fiemap.c.

#ifndef HAVE_IOCTL_1_FIEMAP_FILESYSTEM_H_
#define HAVE_IOCTL_1_FIEMAP_FILESYSTEM_H_

#include "feature-test.h"

#include "attribute.h"
#include "constants.h"

#include <stdio.h>
#include <linux/types.h>
#include <sys/types.h>

ATTRIBUTE((nonnull, returns_nonnull))
FILE *open_file(const char *const path);

// Prints major and minor device numbers and where the device seeems to start.
// Returns where the device seems to start.
__u64 get_offset(const dev_t dev);

#endif // ! HAVE_IOCTL_1_FIEMAP_FILESYSTEM_H_
