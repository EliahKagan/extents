// conf.c - get user-provided configuration for extent table (implementation)

#include "conf.h"

#include "util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// The table's column defaults are LOGICAL, INITIAL, FINAL, COUNT.
#define COLUMNS_DEFAULT_IN_SECTORS "lifc"
#define COLUMNS_DEFAULT_IN_BYTES "LIFC"

// Like it says on the tin. Prints a help message and exits indicating success.
static noreturn void show_help_and_quit(void)
{
    puts("Usage:\n");
    printf("  %s [-t licfLICF] PATH\n", progname());
    printf("  %s -B PATH\n", progname());
    printf("  %s -s PATH\n\n", progname());
    puts("The -t option specifies a full list of columns as follows:\n");
    puts("  l or L   logical offset in file, in sectors (l) or bytes (L)");
    puts("  i or I   initial block on disk, in sectors (i) or bytes (I)");
    puts("  f or F   final block on disk, in sectors (f) or bytes (F)");
    puts("  c or C   count of blocks in file, in sectors (c) or bytes (C)\n");
    puts("The -B option means -t LIFC.");
    puts("The -s option means -t lifc, which is the default.\n");
    puts("Mutliple column specifications don't combine. The last one wins.");

    exit(EXIT_SUCCESS);
}

int get_table_configuration(int argc, char *const *restrict const argv,
                            struct conf *restrict const cp)
{
    // TODO: Maybe refactor this function into multiple functions.
    assert(argc > 0);
    assert(argv);
    assert(cp);

    set_progname(argv[0]);

    cp->columns = COLUMNS_DEFAULT_IN_SECTORS;

    // TODO: Maybe support long options. But I'm unsure whether wise to use
    //       getopt_long(); musl compatibility remains a goal for this program.
    opterr = false;
    for (int opt = 0; (opt = getopt(argc, argv, ":t:Bsh")) != -1; ) {
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
            die("unrecognized option: -%c", optopt);

        default:
            die(BUG("getopt() returned %d '%c'"), opt, opt);
        }
    }

    return optind - 1;
}
