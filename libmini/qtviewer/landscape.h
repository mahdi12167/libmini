#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#define SAFE_DECLARE_STRING(name, max_len) \
        name = new char[max_len + 1];      \
        memset(name, 0, sizeof(#name));

#define SAFE_DELETE_STRING(name) \
        if (name != NULL) delete[] name;

#endif
