#include "lib/logging/MessageLogger.h"
#include "lib/logging/PacketLogger.h"

int main() {
    MessageLogger logger{"main", "main"};
    logger.log_message("test message");
}
