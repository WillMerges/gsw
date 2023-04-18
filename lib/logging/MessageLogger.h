/******************************************************************************
*  Name: MessageLogger.h
*
*  Purpose: Logs system messages
*
*  Author: Will Merges
*
******************************************************************************/

#ifndef MESSAGE_LOGGER_H
#define MESSAGE_LOGGER_H

#include <string>
#include <sys/socket.h>
#include <sys/uio.h>

#include "lib/logging/Logger.h"


// Message Logger type and data declarations
namespace MessageLoggerDecls {
    /// @brief kinds of messages
    typedef enum {
        INFO = 0,   // information message
        WARN,       // warning message
        CRIT,       // critical error message
        NUM_MESSAGE_T
    } message_t;

    /// maps message types to strings
    extern const char* message_str[NUM_MESSAGE_T + 1];

    /// @brief data prepended to log messages
    typedef struct {
        double timestamp;
        message_t type;
    } info_t;

    /// @brief the file path to use for addressing (relative to GSW_HOME)
    static const char* ADDRESS_FILE = "message_log_socket";
};


class MessageLogger : public Logger {
public:
    /// @brief constructor
    /// @param class_name  the name of the class the logger is in
    /// @param func_name   the name of the function the logger is in
    MessageLogger(std::string class_name, std::string func_name);

    /// @brief constructor
    /// @param func_name    the name of the function the logger is in
    MessageLogger(std::string func_name);

    /// @brief log a message
    /// @param msg   the message to log
    /// @param type  the type of message to log
    /// @return
    RetType log_message(std::string msg,
             MessageLoggerDecls::message_t type = MessageLoggerDecls::INFO);

private:
    std::string m_className;
    std::string m_funcName;

    struct timeval m_time;

    MessageLoggerDecls::info_t m_info;
    struct iovec m_vecs[2];
};

#endif
