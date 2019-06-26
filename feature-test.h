// feature-test.h - feature test macros to define above system header includes
//  Used by fiemap.c.

#ifndef HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_
#define HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_

//#error This should stop compilation from succeeding.

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreserved-id-macro"
#endif
#define _POSIX_C_SOURCE 1
#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif // ! HAVE_IOCTL_1_FIEMAP_FEATURE_TEST_H_
