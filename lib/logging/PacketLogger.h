/******************************************************************************
*  Name: PacketLogger.h
*
*  Purpose: Logs received network packets
*
*  Author: Will Merges
*
******************************************************************************/

#ifndef PACKET_LOGGER_H
#define PACKET_LOGGER_H

#include <sys/socket.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/uio.h>

#include "lib/logging/Logger.h"

// Packet Logger type and data declarations
namespace PacketLoggerDecls {
    /// @brief the file path to use for addressing (relative to GSW_HOME)
    static const char* ADDRESS_FILE = "lib/logging/PacketLogger.h";
};

class PacketLogger : public Logger {
public:
    /// @brief constructor
    PacketLogger();

    /// @brief log a packet
    /// @param buff     a buffer containing the packet data
    /// @param len      the length of buff in bytes
    /// @return
    RetType log_packet(uint8_t* buff, size_t len);

private:
    double timestamp;
    struct iovec m_vecs[2];
};

#endif
