// feature-test.h - feature test macros to define above system header includes

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
