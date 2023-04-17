/******************************************************************************
*  Name: main.cpp
*
*  Purpose: The logging daemon
*
*  Author: Will Merges
*
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "lib/time/time.h"
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
    const char* curr_time = time_util::to_string(time_util::now());

    char* gsw_home = getenv("GSW_HOME");
    if(NULL == gsw_home) {
        printf("GSW_HOME environment variable not set, did you run '. setenv'?\n");
        exit(FAILURE);
    }

    std::string path = gsw_home;
    path += "/logs";

    struct stat sb;

    if(stat(path.c_str(), &sb) != 0) {
        printf("Logging directory does not exist, creating at '%s'\n", path.c_str());

        if(-1 == mkdir(path.c_str(), 0700)) {
            perror("Failed to create logging directory");
            exit(FAILURE);
        }
    } else {
        if(!S_ISDIR(sb.st_mode)) {
            printf("The path '%s' exists but is not a directory, nowhere to log files\n", path.c_str());
            exit(FAILURE);
        }
    }

    path += "/";
    path += curr_time;

    printf("Creating new logs at '%s'\n", path.c_str());

    if(-1 == mkdir(path.c_str(), 0700)) {
        perror("Failed to create log directory");
        exit(FAILURE);
    }
}
