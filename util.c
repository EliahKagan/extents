// util.c - utility functions (implementation)
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

extern inline int max(int first, int second);
