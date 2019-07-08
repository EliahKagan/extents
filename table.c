// table.c - drawing a custom table of file extents (implementation)

#include "table.h"

#include "attribute.h"
#include "constants.h"
#include "util.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <linux/fiemap.h>
#include <linux/types.h>
#include <sys/ioctl.h>

struct tablespec *alloc_tablespec(const int col_count)
{
    ASSERT_NONNEGATIVE_INT_FITS_IN_SIZE_T();

    assert(col_count >= 0);

    struct tablespec *const tsp =
            xcalloc(1u, sizeof(struct tablespec)
                            + sizeof(struct colspec) * (size_t)col_count);

    tsp->col_count = col_count;

    return tsp;
}

ATTRIBUTE((nonnull))
static inline __u64 get_raw(const struct fiemap_extent *const fep,
                            const enum datum datum)
{
    assert(fep);

    switch (datum) {
        case datum_logical:         return fep->fe_logical;
        case datum_physical:        return fep->fe_physical;
        case datum_length:          return fep->fe_length;
        case datum_physical_end:    return fep->fe_physical + fep->fe_length;
    }

    die(BUG("unrecognized datum selection"));
}

ATTRIBUTE((nonnull))
static inline __u64 get(const struct fiemap *const fmp,
                        const struct colspec *const csp, const __u32 row_index)
{
    assert(fmp);
    assert(csp);
    assert(row_index < fmp->fm_mapped_extents);

    const __u64 raw = get_raw(&fmp->fm_extents[row_index], csp->datum);
    return (raw + csp->offset) / csp->divisor;
}

ATTRIBUTE((nonnull))
static void set_widths_from_labels(struct tablespec *const tsp)
{
    assert(tsp);
    assert(tsp->col_count >= 0);

    for (int col_index = 0; col_index < tsp->col_count; ++col_index) {
        struct colspec *const csp = &tsp->cols[col_index];
        assert(csp->label);

        const size_t width = strlen(csp->label);
        if (width > (size_t)INT_MAX) die(BUG("label too big to format"));
        csp->width = (int)width;
    }
}

ATTRIBUTE((nonnull))
static void update_widths_from_values(struct tablespec *const tsp)
{
    assert(tsp);
    assert(tsp->col_count >= 0);

    const __u32 row_count = tsp->fmp->fm_mapped_extents;

    for (__u32 row_index = 0u; row_index < row_count; ++row_index) {
        for (int col_index = 0; col_index < tsp->col_count; ++col_index) {
            struct colspec *const csp = &tsp->cols[col_index];
            const __u64 value = get(tsp->fmp, csp, row_index);
            csp->width = max(csp->width, snprintf(NULL, 0u, "%llu", value));
        }
    }
}

void populate_widths(struct tablespec *const tsp)
{
    assert(tsp);
    set_widths_from_labels(tsp);
    update_widths_from_values(tsp);
}

ATTRIBUTE((nonnull))
static void show_labels(const struct tablespec *const tsp)
{
    assert(tsp);
    assert(tsp->gap_width > 0 && tsp->col_count >= 0);

    for (int col_index = 0; col_index < tsp->col_count; ++col_index) {
        const struct colspec *const csp = &tsp->cols[col_index];
        assert(csp->label);
        printf("%*s", tsp->gap_width + csp->width, csp->label);
    }

    putchar('\n');
}

ATTRIBUTE((nonnull))
static void show_all_rows(const struct tablespec *const tsp)
{
    assert(tsp);
    assert(tsp->gap_width > 0 && tsp->col_count >= 0);

    const __u32 row_count = tsp->fmp->fm_mapped_extents;

    for (__u32 row_index = 0u; row_index < row_count; ++row_index) {
        for (int col_index = 0; col_index < tsp->col_count; ++col_index) {
            const struct colspec *const csp = &tsp->cols[col_index];
            const __u64 value = get(tsp->fmp, csp, row_index);
            printf("%*llu", tsp->gap_width + csp->width, value);
        }

        putchar('\n');
    }
}

void show_table(const struct tablespec *const tsp)
{
    assert(tsp);
    show_labels(tsp);
    show_all_rows(tsp);
}
