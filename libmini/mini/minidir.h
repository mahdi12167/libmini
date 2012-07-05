// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDIR_H
#define MINIDIR_H

#ifdef _WIN32
#include <windows.h>
inline void remove(const char *file) {DeleteFile(file);}
inline void rmdir(const char *dir) {RemoveDirectory(dir);}
#endif

namespace minidir {

// specify file search path and pattern (with '*' as single wildcard)
void filesearch(const char *spec);

// find next file matching the search pattern
const char *findfile();

}

using namespace minidir;

#endif
