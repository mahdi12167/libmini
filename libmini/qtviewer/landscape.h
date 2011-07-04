#ifndef LANDSCAPE_H
#define LANDSCAPE_H

#include <mini/miniv4d.h>
#include <QColor>

#define SAFE_DECLARE_STRING(name, max_len) \
        name = new char[max_len + 1]; \
        memset(name, 0, sizeof(#name));

#define SAFE_DELETE_STRING(name) \
        if (name != NULL) delete[] name;

inline miniv4d QColor2Color(QColor& color)
{
  return miniv4d(color.redF(), color.greenF(), color.blueF(), color.alphaF());
}

#endif
