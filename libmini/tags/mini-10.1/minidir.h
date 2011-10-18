// (c) by Stefan Roettger

#ifndef MINIDIR_H
#define MINIDIR_H

namespace minidir {

// specify file search path and pattern (with '*' as single wildcard)
void filesearch(const char *spec);

// find next file matching the search pattern
const char *findfile();

}

using namespace minidir;

#endif
