/******************************************************************************
*  Name: MessageLogger.cpp
*
*  Purpose: Logs system messages
*
*  Author: Will Merges
*
******************************************************************************/

#include "lib/logging/MessageLogger.h"
#include "lib/time/time.h"


/// @brief constructor
/// @param class_name  the name of the class the logger is in
/// @param func_name   the name of the function the logger is in
MessageLogger::MessageLogger(std::string class_name,
                std::string func_name) : Logger(MessageLoggerDecls::ADDRESS_FILE),
                                         m_className(class_name),
                                         m_funcName(func_name) {
    // preset the first vector for vectored I/O to be the information struct
    m_vecs[0].iov_base = (void*)&m_info;
    m_vecs[0].iov_len = sizeof(m_info);
};

/// @brief constructor
/// @param func_name    the name of the function the logger is in
MessageLogger::MessageLogger(std::string func_name)
                                       : Logger(MessageLoggerDecls::ADDRESS_FILE),
                                         m_className(""),
                                         m_funcName(func_name) {
    // preset the first vector for vectored I/O to be the information struct
    m_vecs[0].iov_base = (void*)&m_info;
    m_vecs[0].iov_len = sizeof(m_info);
};

/// @brief log a message
/// @param msg   the message to log
/// @param type  the type of message to log
/// @return
RetType MessageLogger::log_message(std::string msg, MessageLoggerDecls::message_t type) {
    m_info.timestamp = time_util::now();
    m_info.type = type;

    std::string output = "(" + m_className + "::" + m_funcName + ") ";
    output += msg;

    m_vecs[1].iov_base = (void*)(output.c_str());
    m_vecs[1].iov_len = output.size();

    return log_vec(m_vecs, 2);
}
