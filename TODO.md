## To Do

This file lists TODO and FIXME items that aren't commented in the code, are
likely to be missed on casual reading, or are particularly important or
interesting.

- I believe Bash uses `intmax_t` for integers in arithmetic evaluation, in
which case the `stitch` script should work with numbers from real-world disks
just fine. But I need to verify that it does actually do that. I should also
add checks to `stitch` that cover overflow cases, because *(a)* it's supposed
to be a robust parser that identifies obviously unreasonable input and *(b)* in
theory, the range of `intmax_t` could be exceeded because it's signed and
`__u64`—defined in the Linux kernel headers and used by the FIEMAP ioctl—is
unsigned. (*Usually* `intmax_t` and `__u64` will be signed and unsigned types
with the same size and alignment; so `__u64` can usually represent some values
too big for `intmax_t`.)

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
