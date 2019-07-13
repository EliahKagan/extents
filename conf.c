// conf.c - get user-provided configuration for extent table (implementation)

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
#define COLUMNS_DEFAULT_IN_SECTORS "lifc"
#define COLUMNS_DEFAULT_IN_BYTES "LIFC"

enum compile_time_longopts_configuration {
#ifdef NO_LONGOPTS
    k_accept_longopts = 0
#else
    k_accept_longopts = 1
#endif
};

// Like it says on the tin. Prints a help message and exits indicating success.
static noreturn void show_help_and_quit(void)
{
    puts("Usage:\n");
    printf("  %s [-t licfLICF] PATH\n", progname());
    printf("  %s -B PATH\n", progname());
    printf("  %s -s PATH\n\n", progname());

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

    if (k_accept_longopts == (0)) {
        puts("The -B option means -t LIFC.");
        puts("The -s option means -t lifc, which is the default.\n");
    } else {
        puts("The -B (--bytes) option means -t LIFC.");
        puts("The -s (--sectors) option means -t lifc, which is the"
                " default.\n");
    }

    puts("Mutliple column specifications don't combine. The last one wins.");

    exit(EXIT_SUCCESS);
}

static const char *const k_shortopts = ":t:Bsh";

#ifdef NO_LONGOPTS
#define GETOPT(ac, av) (getopt(ac, av, k_shortopts))
#else
static const struct option k_longopts[] = {
    { "table", required_argument, NULL, 't' },
    { "bytes", no_argument, NULL, 'B' },
    { "sectors", no_argument, NULL, 's' },
    { "secs", no_argument, NULL, 's' },
    { "help", no_argument, NULL, 'h' },
    { 0 }
};

#define GETOPT(ac, av) (getopt_long(ac, av, k_shortopts, k_longopts, NULL))
#endif

static noreturn void die_unrecognized_option(char *const *const argv)
{
    if (optopt)
        die("unrecognized option: -%c", optopt);
    else if (k_accept_longopts != (0))
        die("unrecognized option: %s", argv[optind - 1]);
    else
        die(BUG("unrecognized option diagnostic failed"));
}

static void process_option(char *const *restrict const argv, const int opt,
                           struct conf *restrict const cp)
{
    switch (opt) {
    case 't':
        cp->columns = optarg;
        break;

    case 'B':
        cp->columns = COLUMNS_DEFAULT_IN_BYTES;
        break;

    case 's':
        cp->columns = COLUMNS_DEFAULT_IN_SECTORS;
        break;

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

// Reads options (and their operands) from the command-line arguments and
// returns a string of column specifiers for the file extent table.
int get_table_configuration(int argc, char *const *restrict const argv,
                            struct conf *restrict const cp)
{
    // TODO: Maybe extract some parts of this function into other functions.
    assert(argc > 0);
    assert(argv);
    assert(cp);

    set_progname(argv[0]);

    cp->columns = COLUMNS_DEFAULT_IN_SECTORS;

    opterr = false;
    for (int opt = 0; (opt = GETOPT(argc, argv)) != -1; )
        process_option(argv, opt, cp);

    return optind - 1;
}
