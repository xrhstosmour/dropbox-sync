#ifndef DROPBOX_CLIENT_COMMANDPARSER_H
#define DROPBOX_CLIENT_COMMANDPARSER_H

// Standard c libraries.
#include <stdio.h>
#include <stdlib.h>

// For string functions.
#include <string.h>

// For checking if the command line input is a digit.
#include <ctype.h>

// For directories
#include <sys/stat.h>

// For using the isValidIpAddress().
#include "clientUtils.h"

// Prototypes for commandParser.c.
// Match the command line arguments to the specific variables and check them. Return 1 if there was an error and so we must exit program.
int commandParser(int argc, char *argv[], DIR **directoryDIR, char **directoryName, int *portNumber, int *workerThreads, int *bufferSize, int *serverPort, char **serverIp);

#endif //DROPBOX_CLIENT_COMMANDPARSER_H
