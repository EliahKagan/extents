## To Do

This file lists TODO and FIXME items that aren't commented in the code, are
likely to be missed on casual reading, or are particularly important or
interesting.

- Merge `test` into the master branch, since it's ready, and the new
`README.md`file documents its presence!

    What data type Bash uses for integers in arithmetic evaluation was my last
    concern blocking the merge. I had thought it used `long` on all systems.
    But I believe it acutally uses `intmax_t`. I should look into this
    properly, of course, and also it's possible (and highly likely!) for the
    maximum value of `__u64` from the kernel headers to be greater than that of
    `intmax_t`, since `intmax_t` is signed. This case seems unlikely to arise
    from running `fiemap` on a real-life disk, and it could be covered by
    additional checks in `stitch`. While this shouldn't be ignored, I don't
    think it needs to be regarded as a blocking problem, especially since
    `fiemap` and `stitch` are currently unreleased alpha software.

    After the merge, the still-relevant portions to this item can be placed in
    a new item in this file and/or in a revised TODO or FIXME comment in
    `stitch`, whereupon this item should be deleted.

- Put short comments atop each functions briefly describing what it does. A few
functions have this; most don't. Functions prototyped in header files should
have these comments there. Other functions, which are hopefully all marked
static, should have those comments on their definitions, though, for them, the
comments could be omitted in the few cases where they truly contribute no
information.

- `Makefile` should support debug vs. release mode. (`NDEBUG` should *not*
be defined in either case. If the compiler can't verify and optimize away an
assertion, I want that assertion.)

- `Makefile` should support specifying whether or not the build includes
support for long options (using `getopt_long()` instead of `getopt()`). Right
now this can be controlled by defining or undefining a `NO_LONGOPTS` macro.

- Find out why GNU `getopt()` exhibits `POSIXLY_CORRECT`-like behavior with
feature test macros weaker than `_GNU_SOURCE`. See the "TODO" comment in
`feature-test.h`.
