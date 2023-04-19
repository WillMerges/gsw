/******************************************************************************
*  Name: main.cpp
*
*  Purpose: The logging daemon
*
*  Author: Will Merges
*
*  Usage: ./logd [--messages] [--packets rate] [--help]
*
******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/select.h>

#include "lib/time/time.h"
#include "lib/logging/MessageLogger.h"
#include "lib/logging/PacketLogger.h"

#define MAX_LINES_PER_FILE 512  // limit text files to 512 lines
#define MAX_FILE_SIZE (1 << 31) // limit binary files to 2^32 bytes


// ANSI control escape codes for settings colors
#define ANSI_RESET          "\033[0m"
#define ANSI_GREEN_BOLD     "\033[1;32m"
#define ANSI_YELLOW_BOLD    "\033[1;33m"
#define ANSI_RED_BOLD       "\033[1;31m"
#define ANSI_WHITE_BOLD     "\033[1;37m"
#define ANSI_MAGENTA_BOLD   "\033[1;35m"

// maps message type to escape code color settings
const char* message_color[MessageLoggerDecls::NUM_MESSAGE_T] = \
{
    ANSI_GREEN_BOLD,
    ANSI_YELLOW_BOLD,
    ANSI_RED_BOLD
};


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
        filename += "/messages-";
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

            if(len < (ssize_t)sizeof(MessageLoggerDecls::info_t)) {
                // no data received, try again
                printf("Invalid amount of data read from message logging socket, read %li bytes\n", len);
                continue;
            }

            MessageLoggerDecls::info_t* info = (MessageLoggerDecls::info_t*)buff;
            std::string timestamp = time_util::to_string(info->timestamp, true);
            std::string type = MessageLoggerDecls::message_str[info->type];

            const char* msg;
            if(len == sizeof(MessageLoggerDecls::info_t)) {
                // no message
                msg = "";
            } else {
                msg = (char*)(&(buff[sizeof(MessageLoggerDecls::info_t)]));
            }

            std::string csv_line = timestamp + "," + type + "," + msg + "\n";

            if(fwrite(csv_line.c_str(), sizeof(char), csv_line.length(), f) != csv_line.length()) {
                printf("Failed to write message to log file\n");
                continue;
            }
            fflush(f);

            // echo to standard output
            const char* color = message_color[info->type];
            printf("%s [%s%s%s] %s%s%s\n", timestamp.c_str(),
                                           color, type.c_str(), ANSI_RESET,
                                           ANSI_WHITE_BOLD, msg, ANSI_RESET);
            lines++;
        }

        fclose(f);
        file_index++;
    }
}


/// @brief log packets
/// @param dir          the directory to place packet logs
/// @param print_rate   print a message every 'print_rate' packets logged
// TODO check sizes are correct according to configuration?
//      maybe also configure whether to log short packets
void log_packets(const char* dir, size_t print_rate) {
    // open the UNIX socket that system messages are sent to
    int sd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if(-1 == sd) {
        perror("Failed to open packet logging socket");
        exit(FAILURE);
    }

    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;

    // NOTE: we're guaranteed getenv returns non-NULL because we checked in main
    std::string addr_file = getenv("GSW_HOME");
    addr_file += "/";
    addr_file += PacketLoggerDecls::ADDRESS_FILE;
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
        perror("Failed to bind packet logging socket");
        exit(FAILURE);
    }

    size_t file_index = 0;
    while(1) {
        std::string filename = dir;
        filename += "/packets-";
        filename += std::to_string(file_index);
        filename += ".bin";

        FILE* f = fopen(filename.c_str(), "w");
        if(NULL == f) {
            perror("Failed to open new log file");
            exit(FAILURE);
        }

        uint32_t written = 0;
        size_t packets = 0;
        size_t total_packets = 0;
        uint8_t buff[Logger::MAX_LOG_SIZE + sizeof(PacketLoggerDecls::info_t)];

        while(written < (uint32_t)MAX_FILE_SIZE) {
            ssize_t len = recv(sd, buff, Logger::MAX_LOG_SIZE + sizeof(PacketLoggerDecls::info_t), 0);

            if(len < (ssize_t)sizeof(PacketLoggerDecls::info_t)) {
                // no data received, try again
                printf("Invalid amount of data read from packet logging socket, read %li bytes\n", len);
                continue;
            }

            // write everything out
            if((ssize_t)fwrite(buff, sizeof(uint8_t), len, f) != len) {
                printf("Failed to write message message to log file\n");
            }
            fflush(f);

            packets++;
            if(packets >= print_rate) {
                total_packets += packets;
                packets = 0;
                printf("%sReceived %lu packets%s\n", ANSI_MAGENTA_BOLD,
                                                    total_packets,
                                                    ANSI_RESET);
            }

            written += len;
        }

        fclose(f);
        file_index++;
    }
}


/// @brief buffer the output of two file descriptors and output them to stdout
///        when a full line is buffered. The file descriptors should be pipes
///        that are connected to the output of the message logging process and
///        the packet logging process.
/// @param msg  message logger stdout pipe
/// @param pkt  packet logger stdout pipe
/// returns when both pipes have been closed (usually by process exit)
void pipe_output(int msg, int pkt) {
    std::string msg_buff = "";
    std::string pkt_buff = "";

    fd_set set;

    int nfds;
    if(msg > pkt) {
        nfds = msg + 1;
    } else {
        nfds = pkt + 1;
    }

    int exited = 0;
    while(exited < 2) {
        FD_ZERO(&set);
        FD_SET(msg, &set);
        FD_SET(pkt, &set);

        if(-1 == select(nfds, &set, NULL, NULL, NULL)) {
            perror("select failed, trying again");
            continue;
        }

        if(FD_ISSET(msg, &set)) {
            // data on msg
            char chr;
            if(1 != read(msg, &chr, 1)) {
                perror("read failed on on message pipe");
            } else {
                if(EOF == chr) {
                    printf("detected exit of message logger process\n");
                    exited++;
                } else if('\n' == chr) {
                    // dump the buffer
                    printf("%s\n", msg_buff.c_str());
                    msg_buff = "";
                } else {
                    // buffer the character
                    msg_buff += chr;
                }
            }
        }

        if(FD_ISSET(pkt, &set)) {
            // data on pkt
            char chr;
            if(1 != read(pkt, &chr, 1)) {
                perror("read failed on on packet pipe");
            } else {
                if(EOF == chr) {
                    printf("detected exit of packet logger process\n");
                    exited++;
                } else if('\n' == chr) {
                    // dump the buffer
                    printf("%s\n", pkt_buff.c_str());
                    pkt_buff = "";
                } else {
                    // buffer the character
                    pkt_buff += chr;
                }
            }
        }
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

    // create pipes for stdout of sub processes
    int msg_pipes[2];
    if(-1 == pipe(msg_pipes)) {
        perror("Failed to create message logger stdout pipe");
        exit(FAILURE);
    }

    int pkt_pipes[2];
    if(-1 == pipe(pkt_pipes)) {
        perror("Failed to create packet logger stdout pipe");
        exit(FAILURE);
    }

    // kick off a process for the message logger
    pid_t pid = fork();
    if(0 == pid) {
        // swap out stdout with the write end of the pipe
        if(-1 == dup2(msg_pipes[1], 1)) {
            perror("dup2 failed for message logger pipe");
            exit(FAILURE);
        }

        // don't need the read end of the pipe
        close(msg_pipes[0]);

        log_messages(msg_path.c_str());

        // should never get here
        printf("Message logger process returned unexpectedly!\n");
        exit(FAILURE);
    }

    printf("Started message logger process with PID %d\n", pid);

    // kick off a process for the packet logger
    pid = fork();
    if(0 == pid) {
        // swap out stdout with the write end of the pipe
        if(-1 == dup2(pkt_pipes[1], 1)) {
            perror("dup2 failed for packet logger pipe");
            exit(FAILURE);
        }

        // don't need the read end of the pipe
        close(pkt_pipes[0]);

        // TODO have packet reporting rate passed in as a parameter
        log_packets(packets_path.c_str(), 1);

        // should never get here
        printf("Packet logger process returned unexpectedly!\n");
        exit(FAILURE);
    }

    printf("Started packet logger process with PID %d\n", pid);

    // don't need the write ends of the pipes
    close(msg_pipes[1]);
    close(pkt_pipes[1]);

    // pipe the output to stdout
    printf("\n");
    pipe_output(msg_pipes[0], pkt_pipes[0]);

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
