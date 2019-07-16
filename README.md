# extents - *querying and accessing file extents*

*Written in 2019 by Eliah Kagan \<degeneracypressure@gmail.com\>.*

*To the extent possible under law, the author(s) have dedicated all copyright
and related and neighboring rights to this software to the public domain
worldwide. This software is distributed without any warranty.*

*You should have received a copy of the CC0 Public Domain Dedication along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.*

This repository provides source code for the C program `fiemap`, as well as the
shell script `stitch` that understands and uses the ouput of `fiemap`. These
utilities only run on GNU/Linux systems.

`Makefile` contains rules for building `fiemap` and for testing it with
`stitch`.

## `fiemap`

`fiemap` is a C program that uses the FIEMAP ioctl to retrieve a list of
extents for a file stored on a block device whose filesystem supports that
ioctl.

Unlike `hdparm --fibmap`, if that ioctl isn't supported, no fallback is
performed. *Also unlike `hdparm`, the `fiemap` utility need not be run as
root.*

`fiemap` assumes every byte logically contained in a file is present in some
extent, so for sparse files its output is probably never useful.

Although potentially handy, `fiemap` doesn't attempt to detect or diagnose
unusual cases (though it shouldn't *crash* due to them—if it does, that's a
bug). Because this is just an alpha version, you should *definitely* use
`hdparm` instead, unless:

- you need a utility that works when run by a non-root user, *or*
- your goal is to develop, test, and/or sate your curiosity about `fiemap`.

## `stitch`

`stitch` is a Bash script that reads a list of extents in the format
produced by `fiemap` and attempts to stitch the file back together from disk.

Even though `fiemap` doesn't need to be run as root, `stitch` does, because it
directly reads data from a block device. `stitch` does not take the name of the
file and does not use its inode.

`stitch`'s current behavior when it (thinks it) is run by a non-root user is to
stop after it parses its input. Even this is arguably useful, as it still emits
an error if the input isn't in the correct format or presents inconsistent
information.

`stitch` is also *still in alpha testing*, which should give you some pause,
being as it's a 200+ line shell script you run as root to directly access
sectors on your disk. It should never write directly to those blocks, of
course, and I don't *think* I made any big mistakes...

## How to Use

The suggested way to try out `fiemap` and `stitch` is:

1. Build `fiemap` by running `make`.

2. Create a symbolic link in the top-level directory of the repository called
`test-symlink` and point it at a file you want to test `fiemap` (and `stitch`)
with. The file should preferably have multiple extents, though empty files and
single-extent files are supported. A file of a few hundred megabytes is ideal
for testing.

3. Test `fiemap` and `stitch` by running `make test`.

    This runs `fiemap` as you, then uses `sudo` to run `stitch` on the output.
