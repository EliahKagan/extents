// conf.h - get user-provided configuration for extent table
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

#ifndef HAVE_EXTENTS_FIEMAP_CONF_H_
#define HAVE_EXTENTS_FIEMAP_CONF_H_

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
int get_table_configuration(int argc, char **restrict argv,
                            struct conf *restrict cp);

#endif // ! HAVE_EXTENTS_FIEMAP_CONF_H_
