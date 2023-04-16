/******************************************************************************
*  Name: Logger.h
*
*  Purpose: Generic logger
*
*  Author: Will Merges
*
******************************************************************************/

#ifndef LOGGER_H
#define LOGGER_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/un.h>

#include "common/types.h"


// a logger opens a UNIX socket based on a filename for the logging type

// logging messages are sent as packets over a UNIX socket, the logging daemon
// should listen on the other side and log the message to disk (if running)

class Logger {
public:
    /// the maximum number of bytes that can be sent in a log message
    /// NOTE: cannot exceed the UNIX domain socket MTU
    static const size_t MAX_LOG_SIZE = 4096;

    /// @brief constructor
    /// @param filename     a unique filename bound to logging messages
    Logger(const char* filename);

    /// @brief destructor
    virtual ~Logger();

    /// @brief initialize the logger
    /// @param filename     a unique filename bound to logging messages
    /// @return
    RetType init(const char* filename);

    /// @brief log data
    /// @param data     the buffer of bytes to log
    /// @param len      the length of 'data' in bytes
    /// @return
    RetType log(uint8_t* data, size_t len);

private:
    // filename corresponding to logging messages of this type
    const char* m_filename;

    // socket descriptor
    int m_sd;

    // address to send logging messages to
    struct sockaddr_un m_addr;
};

#endif
