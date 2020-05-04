#ifndef DROPBOX_CLIENT_CLIENTUTILS_H
#define DROPBOX_CLIENT_CLIENTUTILS_H

// Buffers for listening size and for read and write data.
#define MAX_PENDING_LISTEN_SIZE 100
#define MAX_BUFFER_SIZE 2048

// For the standard libraries.
#include <stdio.h>
#include <stdlib.h>

// For the sockets.
#include <sys/socket.h>

// For network functions like gethostbyaddr().
#include <netdb.h>

// For SIGINT in order to terminate the program.
#include <signal.h>

// For read() and maybe possible use of forks.
#include <unistd.h>

// For directories.
#include <dirent.h>

// For bool functions and variables.
#include <stdbool.h>

// For using the inet_pton() for valid ip addresses check.
#include <arpa/inet.h>

// For the thread creation and manipulation.
#include <pthread.h>

// Include all the headers we are going to use.
#include "commandParser.h"
#include "tupleList.h"

// Struct for passing everything we need through our thread.
struct threadData
{
    tupleList *clientsList;
    int *clientSocket;
    int *clientPort;
    char *clientIpAddress;
};

// Extern value for our lock, dataLock and shouldRun in order to have access inside the threads.
extern pthread_mutex_t lock;
extern pthread_mutex_t dataLock;
extern bool volatile shouldRun;

// Prototypes for clientUtils.c.
// Terminate the program after freeing everything.
void exitProgram(int status, int clientSocket, DIR **directoryDIR, char *directoryName, char *serverIp, char *clientIpAddress, tupleList *clientsList, struct threadData *data, pthread_t clientThreads[], int workerThreads);

// Terminate the program by calling the exitProgram function on pressing CTRL + C.
void signalHandler(int signal);

// Check if an IPv4 address is valid.
bool isValidIpAddress(char *ipAddress);

// Send first the data length and then the data to the serverSocket.
int sendData(int serverSocket, char *data);

// Receive first the sending data length and then the data from the server or a client.
int receiveData(int socket, char *buffer);

// Get the public ip of the client.
// Link with provided help: https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/.
char *getIpAddres();

// Client's thread which implements all its work flow.
// Link with provided help: https://gist.github.com/oleksiiBobko/43d33b3c25c03bcc9b2b
void *connectionHandler(void *);

// Extract string between 2 patterns. Link with provided help: https://stackoverflow.com/questions/24696113/how-to-find-text-between-two-strings-in-c
char *extractStringBetweenPatterns(char *startPattern, char *endPattern, char *stringToExtractFrom);

#endif //DROPBOX_CLIENT_CLIENTUTILS_H
