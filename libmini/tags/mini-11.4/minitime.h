// (c) by Stefan Roettger, licensed under LGPL 2.1

#ifndef MINITIME_H
#define MINITIME_H

namespace minitime {

double gettime();
void waitfor(double secs);

int is_leapyear(int year);
unsigned int daysofmonth(unsigned int month,int year);
unsigned int daysofyear(int year);

// returns the seconds since 1.1.1970 00:00 (the epoch)
double utc2unixtime(int year=1970,unsigned int month=1,unsigned int day=1,
                    unsigned int hour=0,unsigned int minute=0,unsigned int second=0,
                    double milliseconds=0.0);

// returns the seconds since my birthday on 2.7.1971 14:34
// the difference to unix time since 1.1.1970 is 47313240 seconds
double utc2minitime(int year=1971,unsigned int month=7,unsigned int day=2,
                    unsigned int hour=14,unsigned int minute=34,unsigned int second=0,
                    double milliseconds=0.0);

}

using namespace minitime;

#endif
