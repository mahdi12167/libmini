// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINITIME_H
#define MINITIME_H

namespace minitime {

double gettime();
void waitfor(double secs);

int is_leapyear(int year);
unsigned int daysofmonth(unsigned int month,int year);

}

using namespace minitime;

#endif
