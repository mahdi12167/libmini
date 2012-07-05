// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIDIR_H
#define MINIDIR_H

namespace minidir {

// c-lib wrappers
#ifdef _WIN32
#include <windows.h>
inline void removefile(const char *file) {DeleteFile(file);}
inline void removedir(const char *dir) {RemoveDirectory(dir);}
#else
#include <stdio.h>
#include <unistd.h>
inline void removefile(const char *file) {remove(file);}
inline void removedir(const char *dir) {rmdir(dir);}
#endif

// specify file search path and pattern (with '*' as single wildcard)
void filesearch(const char *spec);

// find next file matching the search pattern
const char *findfile();

}

using namespace minidir;

#endif
