// attribute.h - an ATTRIBUTE() macro for __attribute__() when known supported
//
// This file is part of extents, tools for querying and accessing file extents.
//
// Written in 2019 by Eliah Kagan <degeneracypressure@gmail.com>.
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related and neighboring rights to this software to the public domain
// worldwide. This software is distributed without any warranty.

#ifndef HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_
#define HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_

#ifdef __GNUC__
#define ATTRIBUTE(x) __attribute__(x)
#endif

#endif // ! HAVE_IOCTL_1_FIEMAP_ATTRIBUTE_H_
