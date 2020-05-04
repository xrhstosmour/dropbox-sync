#ifndef DROPBOX_SERVER_LIST_H
#define DROPBOX_SERVER_LIST_H

// Standard c libraries
#include <stdio.h>
#include <stdlib.h>

// For string functions.
#include <string.h>

// tupleNode for storing the data for each client and the nextTupleNode tupleNode.
typedef struct tupleNode
{

    char *ipAddress;
    int portNumber;
    int fdSocketClient;
    struct tupleNode *nextTupleNode;

} tupleNode;

// We will use a linked list for storing our nodes because we do not need any indicators for them:
// Δεν χρειάζεται να υλοποιήσετε κάποια δομή δεικτοδότησης για τη λίστα, όποτε χρειάζεστε κάτι μπορείτε να την ψάχνετε σειριακά.
typedef struct tupleList
{

    struct tupleNode *headTupleNode, *currentTupleNode;
    int size;

} tupleList;

// Prototypes for tupleList.c.
// Create a new tupleNode.
tupleNode *createTupleNode(char *ipAddress, int portNumber);

// Create an empty tupleList.
tupleList *createTupleList();

// Add a tupleNode to the tupleList.
int addToTupleList(tupleList *clients, char *ipAddress, int portNumber, int fdSocketClient);

// Remove a tupleNode from the tupleList given the ipAddress.
void removeFromTupleList(tupleList *clients, char *ipAddress, int portNumber);

//Destroy the created tupleList.
void destroyTupleList(tupleList *clients);

// Check if the given data are the same with the data of the given tupleNode and return 1 otherwise 0.
int isTheSameTupleNode(tupleNode *tupleToCheck, char *ipAddress, int portNumber);

// Check if a tupleNode already exists in our tupleList and return it otherwise return NULL.
tupleNode *findTupleNode(tupleList *clients, char *ipAddress, int portNumber);

#endif //DROPBOX_SERVER_LIST_H
