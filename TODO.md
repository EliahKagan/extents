## To Do

This file lists TODO and FIXME items that aren't commented in the code, are
likely to be missed on casual reading, or are particularly important or
interesting.

- Put `restrict` on pointer parameters where appropriate.

- Find out why GNU `getopt()` exhibits `POSIXLY_CORRECT`-like behavior with
feature test macros weaker than `_GNU_SOURCE`. See the "TODO" comment in
`feature-test.h`.
