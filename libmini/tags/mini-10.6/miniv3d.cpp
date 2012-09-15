// (c) by Stefan Roettger, licensed under LGPL 2.1

#include "miniv3d.h"

#include "miniv3f.h"
#include "miniv4d.h"
#include "miniv4f.h"

#include "ministring.h"

// conversion constructors
miniv3d::miniv3d(const miniv3f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z;}
miniv3d::miniv3d(const miniv4d &v) {x=v.x; y=v.y; z=v.z;}
miniv3d::miniv3d(const miniv4f &v) {x=(double)v.x; y=(double)v.y; z=(double)v.z;}

// string cast operator
miniv3d::operator ministring() const {return((ministring)"(" + x + "," + y + "," + z + ")");}
