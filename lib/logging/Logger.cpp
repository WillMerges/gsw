/******************************************************************************
*  Name: Logger.cpp
*
*  Purpose: Generic logger
*
*  Author: Will Merges
*
******************************************************************************/

#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "lib/logging/Logger.h"

/// @brief constructor
/// @param filename     a unique filename bound to logging messages
Logger::Logger(const char* filename) : m_filename(filename), m_sd(-1) {
    // attempt to initialize
    // don't error if it fails
    init();
};

/// @brief destructor
Logger::~Logger() {
    if(-1 != m_sd) {
        close(m_sd);
    }
}

/// @brief initialize the logger
/// @return
RetType Logger::init() {
    // open the UNIX socket
    m_sd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if(-1 == m_sd) {
        return FAILURE;
    }

    // set the address to send logging messages too
    m_addr.sun_family = AF_UNIX;

    size_t len = 0;
    while(len < (sizeof(m_addr.sun_path) / sizeof(char))) {
        m_addr.sun_path[len] = m_filename[len];

        if('\0' == m_filename[len]) {
            break;
        }

        len++;
    }

    if(len >= (sizeof(m_addr.sun_path) / sizeof(char))) {
        // filename was too long!
        close(m_sd);
        m_sd = -1;

        return FAILURE;
    }

    return SUCCESS;
}

/// @brief log data
/// @param data     the buffer of bytes to log
/// @param len      the length of 'data' in bytes
/// @return
/// NOTE: init must be run and return SUCCESS before log will succeed!
RetType Logger::log(uint8_t* data, size_t len) {
    if(-1 == m_sd) {
        // no socket!
        // init was never run successfully
        return FAILURE;
    }

    if(-1 == sendto(m_sd, data, len, 0,
                    (struct sockaddr*)&m_addr, sizeof(m_addr))) {
        return FAILURE;
    }

    return SUCCESS;
}
