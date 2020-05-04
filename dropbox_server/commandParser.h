#ifndef DROPBOX_SERVER_COMMANDPARSER_H
#define DROPBOX_SERVER_COMMANDPARSER_H

// Standard c libraries.
#include <stdio.h>
#include <stdlib.h>

// For string functions.
#include <string.h>

// For checking if the command line input is a digit.
#include <ctype.h>

// Prototypes for commandParser.c.
// Match the command line arguments to the specific variables and check them. Return 1 if there was an error and so we must exit program.
int commandParser(int argc, char *argv[], int *portNumber);

#endif //DROPBOX_SERVER_COMMANDPARSER_H
