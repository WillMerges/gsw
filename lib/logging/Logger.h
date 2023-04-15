/******************************************************************************
*  Name: Logger.h
*
*  Purpose: Generic logger
*
*  Author: Will Merges
*
******************************************************************************/

#include <stdlib.h>
#include <stdint.h>
#include <sys/un.h>

#include "common/types.h"

#ifndef LOGGER_H
#define LOGGER_H

// a logger opens a UNIX socket based on a filename for the logging type

// logging messages are sent as packets over a UNIX socket, the logging daemon
// should listen on the other side and log the message to disk (if running)

class Logger {
public:
    /// @brief constructor
    /// @param filename     a unique filename bound to logging messages
    Logger(const char* filename);

    /// @brief destructor
    virtual ~Logger();

    /// @brief initialize the logger
    /// @return
    RetType init();

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
