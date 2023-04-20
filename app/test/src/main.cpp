#include <stdint.h>

#include "lib/logging/MessageLogger.h"
#include "lib/logging/PacketLogger.h"

int main() {
    MessageLogger logger{"main", "main"};
    logger.log_message("hello world!", MessageLoggerDecls::INFO);
    logger.log_message("hello world!", MessageLoggerDecls::WARN);
    logger.log_message("hello world!", MessageLoggerDecls::CRIT);

    PacketLogger plogger;
    const char* dat = "hello world!";
    plogger.log_packet((uint8_t*)dat, 12, 8000);
}
