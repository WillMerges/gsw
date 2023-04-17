/******************************************************************************
*  Name: main.cpp
*
*  Purpose: The logging daemon
*
*  Author: Will Merges
*
******************************************************************************/

#include <stdlib.h>

#include "lib/logging/MessageLogger.h"
#include "lib/logging/PacketLogger.h"

#define MAX_LINES_PER_FILE 512  // limit text files to 512 lines
#define MAX_FILE_SIZE (1 << 31) // limit binary files to 2^32 bytes


/// @brief log system messages
void log_messages(bool verbose) {
    // create an output directory

    // open the UNIX socket that system messages are sent to
}


int main() {
    // create the logging directory for this session
    system("date")
}
