#include "lib/logging/MessageLogger.h"

int main() {
    MessageLogger logger{"main", "main"};
    logger.log_message("hello world!", MessageLoggerDecls::INFO);
    logger.log_message("hello world!", MessageLoggerDecls::WARN);
    logger.log_message("hello world!", MessageLoggerDecls::CRIT);
}
