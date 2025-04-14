/// @file DumpUtility.h
/// @brief Provides functions to dump the blocked sequence set file structure.

#ifndef DUMPUTILITY_H
#define DUMPUTILITY_H

#include <string>
#include <fstream>

/// @brief Dumps blocks in physical order (RBN 0 to N) to stdout.
/// @param filename Name of the blocked sequence set file to dump.
void dumpByPhysicalOrder(const std::string& filename);

/// @brief Dumps blocks in logical order (follows linked list of active blocks).
/// @param filename Name of the blocked sequence set file to dump.
void dumpByLogicalOrder(const std::string& filename);

#endif // DUMPUTILITY_H
