// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDIR_H
#define MINIDIR_H

namespace minidir {

// specify file search path and pattern (with '*' as single wildcard)
void filesearch(const char *spec);

// find next file matching the search pattern
const char *findfile();

// check if match is a directory
int isdirectory();

}

using namespace minidir;

#endif
