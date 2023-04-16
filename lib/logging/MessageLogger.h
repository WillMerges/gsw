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
#include <sys/time.h>

#include "lib/logging/Logger.h"

// Message Logger type and data declarations
namespace MessageLoggerDecls {
    /// @brief kinds of messages
    typedef enum {
        INFO_MSG = 0,   // information message
        WARN_MSG,       // warning message
        CRIT_MSG,       // critical error message
        NUM_MESSAGE_T
    } message_t;

    /// @brief maps kinds of messages to a character
    ///        this character will be prepended to every log message to tell
    ///        the log daemon what kind of message it is
    extern char message_type_char[NUM_MESSAGE_T + 1];

    /// @brief maps kinds of messages to a string name
    extern const char* message_type_str[NUM_MESSAGE_T + 1];

    /// @brief the file path to use for addressing (relative to GSW_HOME)
    static const char* ADDRESS_FILE = "lib/logging/PacketLogger.h";
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
             MessageLoggerDecls::message_t type = MessageLoggerDecls::INFO_MSG);

private:
    std::string m_className;
    std::string m_funcName;

    struct timeval m_time;
};

#endif
