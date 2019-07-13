## To Do

This file lists TODO and FIXME items that aren't commented in the code, are
likely to be missed on casual reading, or are particularly important or
interesting.

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
