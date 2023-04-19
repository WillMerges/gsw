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
    static const char* ADDRESS_FILE = "packet_log_socket";

    /// @brief information logged with a packet
    typedef struct {
        uint16_t port;      // destination UDP port in system endianness
        size_t len;         // length of the packet in bytes
        double timestamp;   // UNIX timestamp (seconds since the epoch)
    } info_t;
};

class PacketLogger : public Logger {
public:
    /// @brief constructor
    PacketLogger();

    /// @brief log a packet
    /// @param buff     a buffer containing the packet data
    /// @param port     the UDP destination port of the packet, in system
    ///                 endianness
    /// @param len      the length of buff in bytes
    /// @return
    RetType log_packet(uint8_t* buff, uint16_t port, size_t len);

private:
    PacketLoggerDecls::info_t m_info;
    struct iovec m_vecs[2];
};

#endif
