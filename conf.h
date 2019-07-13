// conf.h - get user-provided configuration for extent table

#ifndef HAVE_IOCTL_1_FIEMAP_CONF_H_
#define HAVE_IOCTL_1_FIEMAP_CONF_H_

#include "feature-test.h"

#include "attribute.h"

// User-provided configuration. Currently this just contains the columns string
// but in future versions it may be expanded with more fields.
struct conf {
    const char *columns;
};

// Parses options and their operands out of command-line arguments using
// getopt(). Doesn't process non-option arguments. Returns an index to the
// first non-option argument or, if there are no such arguments, argc.
ATTRIBUTE((nonnull))
int get_table_configuration(int argc, char *const *restrict argv,
                            struct conf *restrict cp);

#endif // ! HAVE_IOCTL_1_FIEMAP_CONF_H_
