/******************************************************************************
*  Name: time.cpp
*
*  Purpose: Time utilities
*
*  Author: Will Merges
*
******************************************************************************/

#include <time.h>
#include <math.h>

#include "lib/time/time.h"


/// @brief get the number of milliseconds since the epoch in UTC
/// @return the time in milliseconds
double time_util::now() {
    double ms;

    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);

    ms = (double)time.tv_sec * 1000;
    ms += ((double)time.tv_nsec / 1000000);

    return ms;
}

/// @brief convert a timestamp to a string in YYYY-MM-DDTHH:mm:ssZ format
/// @param timestamp    the time in milliseconds since the epoch
/// @param subsecond    whether additional subsecond precision should be included,
///                     making the timestamp in YYYY-MM-DDTHH:mm:ss.fffZ format.
const char* time_util::to_string(double timestamp, bool subsecond) {
    static char buff[4096];

    time_t seconds = timestamp / 1000;
    struct tm* tm = gmtime(&seconds);

    int len = snprintf(buff, 4095, "%04d-%02d-%02dT%02d:%02d:%02d",
                        tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
                        tm->tm_hour, tm->tm_min, tm->tm_sec);

    if(subsecond) {
        // get the decimal seconds in 3 digits
        double remain = timestamp - floor(timestamp);
        remain = round(remain * 1000);
        snprintf(&(buff[len]), 4095 - len, ".%03d", (int)remain); // TODO pad right to 3 zeros
    }

    // make sure always NULL terminated
    buff[4096] = '\0';
    return buff;
}
