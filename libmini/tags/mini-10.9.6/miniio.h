// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINIIO_H
#define MINIIO_H

#include <stdio.h>

class ministring;

namespace miniio {

//! check a file
int checkfile(const char *filename);

//! check a file to be absolute or relative
//!  returns length of absolute path prefix
int checkfilepath(const char *filename);

//! write a RAW file
void writefile(const char *filename,unsigned char *data,long long bytes);

//! read a RAW file
unsigned char *readfile(const char *filename,long long *bytes);

//! read from a RAW file
unsigned char *readfiled(FILE *file,long long *bytes);

//! write a string as RAW file
void writestring(const char *filename,const char *cstr);

//! write a ministring as RAW file
void writeministring(const char *filename,const ministring &str);

//! read a RAW file as string
char *readstring(const char *filename);

//! read a RAW file as ministring
ministring readministring(const char *filename);

//! read a RAW file and compute signature
unsigned int signature(const char *filename);

//! compute a signature
unsigned int signature(const unsigned char *data,long long bytes);

//! get file modification year
int getmodyear(const char *filename);

//! get file modification time relative to year
long long int getmodtime(const char *filename,int year);

//! get relative modification time
//!  positive values mean that file #1 is newer than file #2
long long int getreltime(const char *file1,const char *file2);

// c-lib wrappers
#ifdef _WIN32
#include <stdio.h>
#include <direct.h>
inline int removefile(const char *file) {return(remove(file));}
inline int removedir(const char *dir) {return(_rmdir(dir));}
#else
#include <stdio.h>
#include <unistd.h>
inline int removefile(const char *file) {return(remove(file));}
inline int removedir(const char *dir) {return(rmdir(dir));}
#endif

}

using namespace miniio;

#endif
