#include "lib/logging/MessageLogger.h"

int main() {
    MessageLogger logger{"main", "main"};
    logger.log_message("hello world!");
}
