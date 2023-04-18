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
#include <unistd.h>

#include "lib/time/time.h"
#include "lib/logging/MessageLogger.h"
#include "lib/logging/PacketLogger.h"

#define MAX_LINES_PER_FILE 512  // limit text files to 512 lines
#define MAX_FILE_SIZE (1 << 31) // limit binary files to 2^32 bytes


/// @brief log system messages
/// @param dir  the directory to place message logs
void log_messages(const char* dir) {
    // open the UNIX socket that system messages are sent to
    int sd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if(-1 == sd) {
        perror("Failed to open message logging socket");
        exit(FAILURE);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    // NOTE: we're guaranteed getenv returns non-NULL because we checked in main
    std::string addr_file = getenv("GSW_HOME");
    addr_file += "/";
    addr_file += MessageLoggerDecls::ADDRESS_FILE;
    const char* addr_str = addr_file.c_str();

    size_t len = 0;
    while(len < (sizeof(addr.sun_path) / sizeof(char))) {
        addr.sun_path[len] = addr_str[len];

        if('\0' == addr_str[len]) {
            break;
        }

        len++;
    }

    if(len >= (sizeof(addr.sun_path) / sizeof(char))) {
        // filename was too long!
        printf("Filename '%s' used for UNIX address is too long\n", addr_str);
        exit(FAILURE);
    }

    if(-1 == bind(sd, (struct sockaddr*)&addr, sizeof(addr))) {
        perror("Failed to bind message logging socket");
        exit(FAILURE);
    }

    size_t file_index = 0;
    while(1) {
        std::string filename = dir;
        filename += "/";
        filename += std::to_string(file_index);
        filename += ".csv";

        FILE* f = fopen(filename.c_str(), "w");
        if(NULL == f) {
            perror("Failed to open new log file");
            exit(FAILURE);
        }

        // write the row header
        std::string header = "time,type,message\n";

        if(fwrite(header.c_str(), sizeof(char), header.length(), f) != header.length()) {
            printf("Failed to write row header to message log file\n");
            exit(FAILURE);
        }

        size_t lines = 1;
        uint8_t buff[Logger::MAX_LOG_SIZE + sizeof(MessageLoggerDecls::info_t)];
        while(lines < MAX_LINES_PER_FILE) {
            ssize_t len = recv(sd, buff, Logger::MAX_LOG_SIZE + sizeof(MessageLoggerDecls::info_t), 0);

            if(len < sizeof(MessageLoggerDecls::info_t) + 1) {
                // no data received, try again
                printf("Invalid amount of data read from message logging socket, read %d bytes\n", read);
                continue;
            }

            MessageLoggerDecls::info_t* info = (MessageLoggerDecls::info_t*)buff;
            std::string timestamp = time_util::to_string(info->timestamp, true);
            std::string type = MessageLoggerDecls::message_str[info->type];

            char* msg = (char*)(&(buff[sizeof(MessageLoggerDecls::info_t)]));
            std::string csv_line = timestamp + "," + type + "," + msg + "\n";

            if(fwrite(csv_line.c_str(), sizeof(char), csv_line.length(), f) != csv_line.length()) {
                printf("Failed to write message to log file\n");
                continue;
            }
            fflush(f);

            // TODO echo to standard output
            // pretty print it
            printf("%s", csv_line.c_str());


            lines++;
        }

        fclose(f);
        file_index++;
    }
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

    std::string log_path = path + "/" + curr_time;
    printf("Creating new logs at '%s'\n", log_path.c_str());

    if(-1 == mkdir(log_path.c_str(), 0700)) {
        perror("Failed to create log directory");
        exit(FAILURE);
    }

    // create a directory for system messages
    std::string msg_path = log_path + "/messages";

    if(-1 == mkdir(msg_path.c_str(), 0700)) {
        perror("Failed to create 'messages' sub directory");
        exit(FAILURE);
    }

    std::string packets_path = log_path + "/packets";

    if(-1 == mkdir(packets_path.c_str(), 0700)) {
        perror("Failed to create 'packets' sub directory");
        exit(FAILURE);
    }


    // update 'current' sym link
    std::string link_path = path + "/current";

    // remove it if it already exists
    // don't check for error since it may not exist
    remove(link_path.c_str());

    if(-1 == symlink(log_path.c_str(), link_path.c_str())) {
        perror("Failed to update 'current' symbolic link");
        exit(FAILURE);
    }

    // TODO set up pipes so stdout of sub processes gets serialized line-by-line and printed to this stdout
    // TODO kick off processes to log system messages and packets

    log_messages(msg_path.c_str());

    // wait for exit of those processes

    printf("\nAll logging processes exited, cleaning up\n");

    // remove the 'current' sym link
    if(-1 == remove(link_path.c_str())) {
        perror("Failed to remove 'current' sym link");
        exit(FAILURE);
    }

    // update the 'latest' sym link
    link_path = path + "/latest";

    if(-1 == symlink(log_path.c_str(), link_path.c_str())) {
        perror("Failed to update 'latest' symbolic link");
        exit(FAILURE);
    }

    printf("Done\n");
}
