/******************************************************************************
*  Name: PacketLogger.cpp
*
*  Purpose: Logs received network packets
*
*  Author: Will Merges
*
******************************************************************************/

#include "lib/logging/PacketLogger.h"

/// @brief constructor
PacketLogger::PacketLogger() : Logger(PacketLoggerDecls::ADDRESS_FILE) {};

/// @brief log a packet
/// @param buff     a buffer containing the packet data
/// @param len      the length of buff in bytes
/// @return
RetType PacketLogger::log_packet(uint8_t* buff, size_t len) {
    // just wraps log
    return log(buff, len);
}
