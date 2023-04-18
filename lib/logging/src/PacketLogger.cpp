/******************************************************************************
*  Name: PacketLogger.cpp
*
*  Purpose: Logs received network packets
*
*  Author: Will Merges
*
******************************************************************************/

#include "lib/logging/PacketLogger.h"
#include "lib/time/time.h"

/// @brief constructor
PacketLogger::PacketLogger() : Logger(PacketLoggerDecls::ADDRESS_FILE) {
    // always send a timestamp before the packet data
    m_vecs[0].iov_base = (void*)&timestamp;
    m_vecs[0].iov_len = sizeof(timestamp);
};

/// @brief log a packet
/// @param buff     a buffer containing the packet data
/// @param len      the length of buff in bytes
/// @return
RetType PacketLogger::log_packet(uint8_t* buff, size_t len) {
    timestamp = time_util::now();

    m_vecs[1].iov_base = (void*)buff;
    m_vecs[1].iov_len = len;

    return log_vec(m_vecs, 2);
}
