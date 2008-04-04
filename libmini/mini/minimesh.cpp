// (c) by Stefan Roettger

#include "minimesh.h"

// default constructor
minimesh::minimesh(): minidyna<minitet>() {}

// destructor
minimesh::~minimesh() {}

// default constructor
minibspt::minibspt() {}

// destructor
minibspt::~minibspt() {}

// insert from tetrahedral mesh
void minibspt::insert(const minimesh &mesh)
   {
   }

// extract to tetrahedral mesh
void minibspt::extract(minimesh &mesh)
   {
   }
