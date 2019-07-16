// conf.c - get user-provided configuration for extent table (implementation)
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

#include "conf.h"

#include "util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef NO_LONGOPTS
#include <getopt.h>
#endif

// The table's column defaults are LOGICAL, INITIAL, FINAL, COUNT.
static const char *const k_columns_default_in_sectors = "lifc";
static const char *const k_columns_default_in_bytes = "LIFC";

enum compile_time_longopts_configuration {
#ifdef NO_LONGOPTS
    k_accept_longopts = 0
#else
    k_accept_longopts = 1
#endif
};

// Prints brief version information and exits indicating success.
static noreturn void show_version_and_quit(void)
{
    puts("fiemap, v0.1 (alpha)");
    exit(EXIT_SUCCESS);
}

// Prints a help message and exits indicating success.
static noreturn void show_help_and_quit(void)
{
    puts("Usage:\n");

    printf("  %s [-t licfLICF] PATH\n", progname());
    printf("  %s -B PATH\n", progname());
    printf("  %s -s PATH\n", progname());
    printf("  %s { -V | -h }\n\n", progname());

    if (k_accept_longopts == (0)) {
        puts("The -t option specifies a full list of columns as follows:\n");
    } else {
        puts("The -t (--table) option specifies a full list of columns as"
                " follows:\n");
    }

    puts("  l or L   logical offset in file, in sectors (l) or bytes (L)");
    puts("  i or I   initial block on disk, in sectors (i) or bytes (I)");
    puts("  f or F   final block on disk, in sectors (f) or bytes (F)");
    puts("  c or C   count of blocks in file, in sectors (c) or bytes (C)\n");

    puts("Mutliple column specifications don't combine. The last one wins.\n");

    if (k_accept_longopts == (0)) {
        puts("The -B option means -t LIFC.");
        puts("The -s option means -t lifc, which is the default.");
        puts("The -V option prints brief version information.");
        puts("The -h option prints this help message.\n");
    } else {
        puts("The -B (--bytes) option means -t LIFC.");
        puts("The -s (--sectors) option means -t lifc, which is the default.");
        puts("The -V (--version) option prints brief version information.");
        puts("The -h (--help) option prints this help message.");
    }

    exit(EXIT_SUCCESS);
}

// Short options this program accepts, in the getopt() shortopts notation.
static const char *const k_shortopts = ":t:BsVh";

#ifdef NO_LONGOPTS
// Processes short options.
#define GETOPT(ac, av) (getopt(ac, av, k_shortopts))
#else
static const struct option k_longopts[] = {
    { "table", required_argument, NULL, 't' },
    { "bytes", no_argument, NULL, 'B' },
    { "sectors", no_argument, NULL, 's' },
    { "secs", no_argument, NULL, 's' },
    { "version", no_argument, NULL, 'V' },
    { "help", no_argument, NULL, 'h' },
    { 0 }
};

// Processes short and long options.
#define GETOPT(ac, av) (getopt_long(ac, av, k_shortopts, k_longopts, NULL))
#endif

// Prints an error about an unrecognized command-line option flag, and quits.
static noreturn void die_unrecognized_option(char *const *const argv)
{
    if (optopt)
        die("unrecognized option: -%c", optopt);
    else if (k_accept_longopts != (0))
        die("unrecognized option: %s", argv[optind - 1]);
    else
        die(BUG("unrecognized option diagnostic failed"));
}

// Process a single command-line option, including its operand(s) if any.
static void process_option(char *const *restrict const argv, const int opt,
                           struct conf *restrict const cp)
{
    switch (opt) {
    case 't':
        cp->columns = optarg;
        break;

    case 'B':
        cp->columns = k_columns_default_in_bytes;
        break;

    case 's':
        cp->columns = k_columns_default_in_sectors;
        break;

    case 'V':
        show_version_and_quit();

    case 'h':
        show_help_and_quit();

    case ':':
        die("missing operand for -%c option", optopt);

    case '?':
        die_unrecognized_option(argv);

    default:
        die(BUG("getopt() returned %d '%c'"), opt, opt);
    }
}

int get_table_configuration(int argc, char **restrict const argv,
                            struct conf *restrict const cp)
{
    assert(argc > 0);
    assert(argv);
    assert(cp);

    set_progname(argv[0]);

    cp->columns = k_columns_default_in_sectors;

    opterr = false;
    for (int opt = 0; (opt = GETOPT(argc, argv)) != -1; )
        process_option(argv, opt, cp);

    return optind - 1;
}
