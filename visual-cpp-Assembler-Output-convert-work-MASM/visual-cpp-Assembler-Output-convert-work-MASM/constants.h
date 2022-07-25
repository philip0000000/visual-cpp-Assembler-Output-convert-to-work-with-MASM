#pragma once
#ifndef CONTENT_H
#define CONTENT_H


// Project
#define VERSION "1.0.0"
#define PROJECT_NAME "visual-cpp-Assembler-Output-convert-work-MASM"


// Console output
#define NEWLINE '\n'
#define TAB '\t'
#define SPACE ' '
#define QUOTATION_MARK '\"'


// write to file
#define CR '\x0D'						// carriage return
#define LF '\x0A'						// line feed
#define ZERO_TERMINATED_CHARACTER '\0'	// zero-terminated character(null-terminated string)


// searching for in .log file
#define SEARCHING_LOG_FILE      "      Searching "
#define SEARCHING_LOG_FILE_SIZE (sizeof(SEARCHING_LOG_FILE) - 1)
#define FOUND_LOG_FILE          "        Found "
#define FOUND_LOG_FILE_SIZE     (sizeof(FOUND_LOG_FILE) - 1)


// Compile value
#define DEFAULT_COMPILE_VALUE      "/link /subsystem:console /entry:main"
#define UNRESOLVED_EXTERNAL_SYMBOL "unresolved external symbol "
#define DEFAULTLIB				   "/defaultlib:"
#define ERROR_MSG				   "error"


#endif /* CONTENT_H */
