#ifndef DROPBOX_SERVER_SERVERUTILS_H
#define DROPBOX_SERVER_SERVERUTILS_H

// Buffers for pending connections and for read and write data.
#define MAX_PENDING_LISTEN_SIZE 100
#define MAX_CLIENTS_SIZE 30
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

// Include all the headers we are going to use.
#include "commandParser.h"
#include "tupleList.h"

// For read() and maybe possible use of forks.
#include <unistd.h>

// For bool variables.
#include <stdbool.h>

// For using the inet_pton() for valid ip addresses check.
#include <arpa/inet.h>

// For error handling in select socket function.
#include <errno.h>

// For thread creation and manipulation.
#include <pthread.h>

// Extern value for our threadCounter and shouldRun in order to have access inside the threads.
extern volatile int threadCounter;
extern bool volatile shouldRun;

// Struct for passing everything we need through our thread.
struct threadData
{
    tupleList *clientsList;
    int *serverSocket;
    int *acceptedClient;
};

// Prototypes for serverUtils.c.
// Terminate the program after freeing everything.
void exitProgram(int status, int serverSocket, tupleList *clientsList, struct threadData *data, pthread_t serverThread[]);

// Terminate the program by calling the exitProgram function on pressing CTRL + C.
void signalHandler(int signal);

// Manipulate the connection requests for each client.
int acceptConnectionRequest(int request, struct sockaddr *address, socklen_t *addressLength, int clientSockets[]);

// Receive first the sending data length and then the data from the client.
int receiveData(int request, char *buffer);

// Send first the data length and then the data to the client.
int sendData(int serverSocket, char *data);

// Extract string between 2 patterns. Link with provided help: https://stackoverflow.com/questions/24696113/how-to-find-text-between-two-strings-in-c
char *extractStringBetweenPatterns(char *startPattern, char *endPattern, char *stringToExtractFrom);

// Server's thread which implements all its work flow.
// Link with provided help: https://gist.github.com/oleksiiBobko/43d33b3c25c03bcc9b2b
void *connectionHandler(void *);

#endif //DROPBOX_SERVER_SERVERUTILS_H
