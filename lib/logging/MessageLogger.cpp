/******************************************************************************
*  Name: MessageLogger.cpp
*
*  Purpose: Logs system messages
*
*  Author: Will Merges
*
******************************************************************************/

#include "lib/logging/MessageLogger.h"

/// @brief maps kinds of messages to a character
///        this character will be prepended to every log message to tell
///        the log daemon what kind of message it is
char MessageLoggerDecls::message_type_char[MessageLoggerDecls::NUM_MESSAGE_T + 1] = \
{
    'I',    // information message
    'W',    // warning message
    'C',    // critical error message
    'U'     // unknown type
};

/// @brief maps kinds of messages to a string name
const char* MessageLoggerDecls::message_type_str[MessageLoggerDecls::NUM_MESSAGE_T + 1] = \
{
    "INFO", // information message
    "WARN", // warning message
    "CRIT", // critical error message
    "UNKNOWN"
};


/// @brief constructor
/// @param class_name  the name of the class the logger is in
/// @param func_name   the name of the function the logger is in
MessageLogger::MessageLogger(std::string class_name,
                std::string func_name) : Logger(MessageLoggerDecls::ADDRESS_FILE),
                                         m_className(class_name),
                                         m_funcName(func_name) {};

/// @brief constructor
/// @param func_name    the name of the function the logger is in
MessageLogger::MessageLogger(std::string func_name)
                                       : Logger(MessageLoggerDecls::ADDRESS_FILE),
                                         m_className(""),
                                         m_funcName(func_name) {};

/// @brief log a message
/// @param msg   the message to log
/// @param type  the type of message to log
/// @return
RetType MessageLogger::log_message(std::string msg, MessageLoggerDecls::message_t type) {
    std::string output = "";
    output += MessageLoggerDecls::message_type_char[type];

    gettimeofday(&m_time, NULL);
    output += "[" + std::to_string(m_time.tv_sec) + "."
                  + std::to_string(m_time.tv_usec) + "] ";

    output += "(" + m_className + "::" + m_funcName + ") ";
    output += msg;

    printf("%s\n", output.c_str());
    return log((uint8_t*)(output.c_str()), output.size());
}
