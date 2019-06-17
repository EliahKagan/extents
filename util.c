// util.c - utility functions

#include "util.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static const char *g_progname;

void set_progname(const char *const name)
{
    assert(name);
    assert(!g_progname);
    g_progname = name;
}

const char *progname(void)
{
    assert(g_progname);
    return g_progname;
}

noreturn void die(const char *restrict const format, ...)
{
    assert(format);
    fprintf(stderr, "%s: error: ", progname());

    va_list vlist;
    va_start(vlist, format);
    vfprintf(stderr, format, vlist);
    va_end(vlist);

    putc('\n', stderr);

    exit(EXIT_FAILURE);
}

void *xcalloc(const size_t count, const size_t size)
{
    void *const ret = calloc(count, size);
    if (!ret) die("out of memory");
    return ret;
}
