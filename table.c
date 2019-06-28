// table.c - drawing a custom table of file extents (implementation)

#include "table.h"

#include "attribute.h"
#include "constants.h"

#include <assert.h>
#include <stdio.h>
#include <linux/fiemap.h>
#include <sys/ioctl.h>

enum format_widths
{
    k_logical_width = 11,
    k_logical_short_width = 8,
    k_physical_width = 13,
    k_physical_short_width = 10,
    k_length_width = 9,
    k_length_short_width = 6
};

static void show_labels(void)
{
    printf("\n%*s     %*s   %*s     %*s   %*s\n",
           k_logical_width, "LOGICAL", k_logical_short_width, "",
           k_physical_width, "PHYSICAL", k_physical_short_width, "",
           k_length_width, "LENGTH");
}

ATTRIBUTE((nonnull(1)))
static void show_extent(const struct fiemap_extent *const fep,
                        const __u64 offset)
{
    assert(fep);
    const __u64 physical = offset + fep->fe_physical;

    printf("%*llu B = %*llu   %*llu B = %*llu   %*llu B = %*llu\n",
           k_logical_width, fep->fe_logical,
           k_logical_short_width, fep->fe_logical / k_sector_size,
           k_physical_width, physical,
           k_physical_short_width, physical / k_sector_size,
           k_length_width, fep->fe_length,
           k_length_short_width, fep->fe_length / k_sector_size);
}

void show_extent_table(const struct fiemap *const fmp, const __u64 offset)
{
    assert(fmp);

    show_labels();
    for (__u32 i = 0u; i < fmp->fm_mapped_extents; ++i)
        show_extent(&fmp->fm_extents[i], offset);
}
