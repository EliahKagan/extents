// attribute.h - an ATTRIBUTE() macro for __attribute__() when known supported

#ifndef HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_
#define HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_

#ifdef __GNUC__
#define ATTRIBUTE(x) __attribute__(x)
#endif

#endif // ! HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_
