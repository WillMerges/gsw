/******************************************************************************
*  Name: time.h
*
*  Purpose: Time utilities
*
*  Author: Will Merges
*
******************************************************************************/

#ifndef TIME_H
#define TIME_H

#include <stdint.h>
#include <string>

/// @brief get the number of seconds since the epoch in UTC time
/// @return the time in seconds
double timestamp();

/// @brief convert a timestamp to a string in YYYY-MM-DDTHH:mm:ssZ format
/// @param timestamp    the time in seconds since the epoch
/// @param subsecond    whether additional subsecond precision should be included,
///                     making the timestamp in YYYY-MM-DDTHH:mm:ss.fZ format.
const char* timestamp_to_string(double timestamp, bool subsecond=false);

#endif
