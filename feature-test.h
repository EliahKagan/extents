// feature-test.h - feature test macros to define above system header includes
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

#ifndef HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_
#define HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
// TODO: Find out why GNU getopt() doesn't process options that appear after
//       non-option arguments with _POSIX_C_SOURCE 2 or _XOPEN_SOURCE, even
//       when the POSIXLY_CORRECT environment variable is undefined at runtime
//       and optstring has no leading '+'. _GNU_SOURCE will be required for
//       getopt_long() anyway, but it would be useful to know why it exhibits
//       this seemingly undocumented behavior. I do want to support non-leading
//       options wherever feasible (so definitely when glibc is used).
#define _GNU_SOURCE
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // ! HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_
