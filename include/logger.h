#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

/// @brief Initialize the Logger.
/// @note called by engine on start
void logger_init(int logLevel);

/// @brief Write message to log file
/// @param encapsulationDepth Amount of '>' and '<' to place message between
/// @param tabDepth Amount of tab spaces at start of write.
/// @param message Message to write.
/// @param error Append *ERROR* to start of message.
void logger_write(int encapsulationDepth, int tabDepth, const char* message, bool error);

/// @brief deallocate logger resources
void logger_free();

#endif // LOGGER_H
