// conf.c - get user-provided configuration for extent table (implementation)

#include "conf.h"

#include "util.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

static void help(void)
{
    // FIXME: Actually print a help message before returning.
}

int get_table_configuration(int argc, char *const *const argv,
                            struct conf *const cp)
{
    // TODO: Maybe refactor this function into multiple functions.
    assert(argc > 0);
    assert(argv);
    assert(cp);

    set_progname(argv[0]);

    cp->columns = "lifc"; // Default config: LOGICAL, INITIAL, FINAL, COUNT.

    // TODO: Maybe support long options. But I'm unsure whether wise to use
    //       getopt_long(); musl compatibility remains a goal for this program.
    opterr = false;
    for (int opt = 0; (opt = getopt(argc, argv, ":t:h")) != -1; ) {
        switch (opt) {
        case 't':
            cp->columns = optarg;
            break;

        case 'h':
            help();
            exit(EXIT_SUCCESS);

        case ':':
            // FIXME: Check that optopt has the value I think it has here.
            die("missing operand for -%c option", optopt);

        case '?':
            die("unrecognized option: -%c", optopt);

        default:
            die(BUG("getopt() returned %d '%c'"), opt, opt);
        }
    }

    return optind - 1;
}
