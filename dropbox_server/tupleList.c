#include "tupleList.h"

// Create a new tupleNode.
struct tupleNode *createTupleNode(char *ipAddress, int portNumber)
{

    // Allocate memory for the tupleNode.
    tupleNode *client = malloc(sizeof(tupleNode));
    if (client == NULL)
    {

        printf("\n There was an error while creating a tupleNode! \n");

        // Return NULL in order to know that we must exit program.
        return NULL;
    }

    // Pass the portNumber and initialize the nextTupleNode with NULL
    // Allocate memory for the ipAddress char pointer and pass the value.
    client->ipAddress = strdup(ipAddress);
    client->portNumber = portNumber;
    client->fdSocketClient = -1;
    client->nextTupleNode = NULL;

    return client;
}

// Create an empty tupleList.
struct tupleList *createTupleList()
{

    // Allocate memory for tupleList.
    tupleList *clients = malloc(sizeof(tupleList));
    if (!clients)
    {

        printf("\n There was an error while creating the tupleList! \n");

        // Return NULL in order to know that we must exit program.
        return NULL;
    }

    // Initialize the headTupleNode and currentTupleNode with NULL and size with 0.
    clients->headTupleNode = clients->currentTupleNode = NULL;
    clients->size = 0;

    return clients;
}

// Add a tupleNode to the tupleList.
int addToTupleList(tupleList *clients, char *ipAddress, int portNumber, int fdSocketClient)
{

    // If there is not another tupleNode with the same data in our tupleList we must proceed.
    if (!findTupleNode(clients, ipAddress, portNumber))
    {

        // Create a new tupleNode.
        tupleNode *client = createTupleNode(ipAddress, portNumber);
        if (client == NULL)
        {
            printf("\n There was an error while creating a tupleNode! \n");

            // Return 1 in order to know that we must exit program.
            return 1;
        }

        // If tupleList is empty, then the new tupleNode is both headTupleNode and currentTupleNode.
        if (clients->currentTupleNode == NULL)
        {

            // Allocate memory for the tupleNode.
            clients->headTupleNode = malloc(sizeof(tupleNode));
            if (clients->headTupleNode == NULL)
            {

                printf("\n There was an error while creating a tupleNode! \n");

                // Free the ipAddress of the client returned from createTupleNode() because we malloced in order to create enough space for the string.
                free(client->ipAddress);
                client->ipAddress = NULL;

                // Free the tupleNode we created at the start.
                free(client);

                // Return 1 in order to know that we must exit program.
                return 1;
            }

            // Pass the data from the client we created at the start.
            clients->headTupleNode->ipAddress = strdup(client->ipAddress);
            clients->headTupleNode->portNumber = client->portNumber;
            clients->headTupleNode->fdSocketClient = fdSocketClient;
            clients->headTupleNode->nextTupleNode = NULL;

            // currentTupleNode should point to our new headTupleNode.
            clients->currentTupleNode = clients->headTupleNode;

            printf("\n Client <%s, %d> added to the list! \n", clients->currentTupleNode->ipAddress, clients->currentTupleNode->portNumber);

            // Increase the size.
            clients->size += 1;

            // Free the ipAddress of the client returned from createTupleNode() because we malloced in order to create enough space for the string.
            free(client->ipAddress);
            client->ipAddress = NULL;

            // Free the tupleNode we created at the start.
            free(client);

            // If we reach that point it means that everything executed fine.
            return 0;
        }

        // Allocate memory for the tupleNode.
        clients->currentTupleNode->nextTupleNode = malloc(sizeof(tupleNode));
        if (clients->currentTupleNode->nextTupleNode == NULL)
        {

            printf("\n There was an error while creating a tupleNode! \n");

            // Free the ipAddress of the client returned from createTupleNode() because we malloced in order to create enough space for the string.
            free(client->ipAddress);
            client->ipAddress = NULL;

            // Free the tupleNode we created at the start.
            free(client);

            // Return 1 in order to know that we must exit program.
            return 1;
        }

        // Pass the data from the client we created at the start.
        clients->currentTupleNode->nextTupleNode->ipAddress = strdup(client->ipAddress);
        clients->currentTupleNode->nextTupleNode->portNumber = client->portNumber;
        clients->currentTupleNode->nextTupleNode->fdSocketClient = fdSocketClient;
        clients->currentTupleNode->nextTupleNode->nextTupleNode = NULL;

        // currentTupleNode should point to the new tupleNode.
        clients->currentTupleNode = clients->currentTupleNode->nextTupleNode;

        printf("\n Client <%s, %d> added to the list! \n", clients->currentTupleNode->ipAddress, clients->currentTupleNode->portNumber);

        // Increase the size.
        clients->size += 1;

        // Free the ipAddress of the client returned from createTupleNode() because we malloced in order to create enough space for the string.
        free(client->ipAddress);
        client->ipAddress = NULL;

        // Free the tupleNode we created at the start.
        free(client);
    }
    else
    {

        // Return -1 in order to know that the client already exists in our list.
        return -1;
    }

    // If we reach that point it means that everything executed fine.
    return 0;
}

// Remove a tupleNode from the tupleList given the ipAddress.
void removeFromTupleList(tupleList *clients, char *ipAddress, int portNumber)
{

    // If there is something in our list to remove.
    if (clients == NULL)
    {
        return;
    }

    // If the tupleNode to be deleted is headTupleNode.
    if (isTheSameTupleNode(clients->headTupleNode, ipAddress, portNumber))
    {

        // If the tupleNode to be deleted is headTupleNode an the only tupleNode of the tupleList.
        if (clients->headTupleNode->nextTupleNode == NULL)
        {

            // Free the ipAddress because we malloced in order to create enough space for the string.
            free(clients->headTupleNode->ipAddress);
            clients->headTupleNode->ipAddress = NULL;

            // Free the tupleNode.
            free(clients->headTupleNode);

            // Initialize the headTupleNode and currentTupleNode with NULL.
            clients->headTupleNode = clients->currentTupleNode = NULL;

            printf("\n Client <%s, %d> removed from the list! \n", ipAddress, portNumber);

            // Decrease the size.
            clients->size -= 1;

            return;
        }

        // Here we are if the tupleNode to be deleted is not the only tupleNode in our tupleList.
        // Keep the tupleNode which we are going to delete after passing every data to the headTupleNode.
        tupleNode *tupleToBeDeleted = clients->headTupleNode;

        // Assign the head to the next tupleNode of the one we are going to delete.
        clients->headTupleNode = clients->headTupleNode->nextTupleNode;

        // Free the ipAddress because we malloced in order to create enough space for the string.
        free(tupleToBeDeleted->ipAddress);
        tupleToBeDeleted->ipAddress = NULL;

        // Free the tupleNode.
        free(tupleToBeDeleted);

        printf("\n Client <%s, %d> removed from the list! \n", ipAddress, portNumber);

        // Decrease the size.
        clients->size -= 1;

        return;
    }

    // Here we are if the tupleNode to be deleted is not the headTupleNode
    // Variable in order to keep the previousClient of the tupleNode we are going to delete.
    tupleNode *previousClient = clients->headTupleNode;

    // While there are available tupleNode and they are not the one we are searching for we must move on to the next tupleNode.
    while (previousClient->nextTupleNode != NULL &&
           !isTheSameTupleNode(previousClient->nextTupleNode, ipAddress, portNumber))
    {

        // Go to the next tupleNode of the tupleList.
        previousClient = previousClient->nextTupleNode;
    }

    // Check if tupleNode really exists in our tupleList.
    if (previousClient->nextTupleNode == NULL)
    {
        // Return the asked error if the client does not exist in our list.
        printf("\n ERROR_IP_PORT_NOT_FOUND_IN_LIST \n");
        return;
    }

    // If the tupleNode to be deleted is the currentTupleNode we must assign as currentTupleNode the previous one.
    if (isTheSameTupleNode(previousClient->nextTupleNode, clients->currentTupleNode->ipAddress,
                           clients->currentTupleNode->portNumber))
    {

        // Set as currentTupleNode the previousClient.
        clients->currentTupleNode = previousClient;
    }

    // Keep the tupleNode we are going to delete in a pointer in order to free it later.
    tupleNode *tupleNodeToDelete = previousClient->nextTupleNode;

    // Remove the link of the tupleNode to be deleted with its nextTupleNode by passing it to the previousClient's nextTupleNode.
    previousClient->nextTupleNode = previousClient->nextTupleNode->nextTupleNode;

    // Free the ipAddress because we malloced in order to create enough space for the string.
    free(tupleNodeToDelete->ipAddress);
    tupleNodeToDelete->ipAddress = NULL;

    // Free the tupleNode.
    free(tupleNodeToDelete);

    printf("\n Client <%s, %d> removed from the list! \n", ipAddress, portNumber);

    // Decrease the size.
    clients->size -= 1;

    return;
}

//Destroy the created tupleList.
void destroyTupleList(tupleList *clients)
{

    // Check if the tupleList is not null before getting the headTupleNode in order to avoid: Invalid read of size 8.
    if (clients == NULL)
    {

        return;
    }

    // Variable for keeping the headTupleNode in order to go through all the tupleNodes list without changing the clients->headTupleNode.
    tupleNode *client;

    while (clients->headTupleNode != NULL)
    {

        // Keep the headTupleNode.
        client = clients->headTupleNode;

        // Go to the next tupleNode of the tupleList.
        clients->headTupleNode = clients->headTupleNode->nextTupleNode;

        // Free the ipAddress because we malloced in order to create enough space for the string.
        free(client->ipAddress);
        client->ipAddress = NULL;

        // Free the current tupleNode.
        free(client);
    }

    // Finally free the tupleList.
    free(clients);
}

// Check if the given data are the same with the data of the given tupleNode and return 1 otherwise 0.
int isTheSameTupleNode(tupleNode *tupleToCheck, char *ipAddress, int portNumber)
{

    // If the tupleNode's data are the same with the given ones return 1;
    if ((strcmp(tupleToCheck->ipAddress, ipAddress) == 0) && (tupleToCheck->portNumber == portNumber))
    {

        return 1;
    }

    // If we are here it means that the tupleNode's are not the same as the given ones and so we must return 0;
    return 0;
}

// Check if a tupleNode already exists in our tupleList and return it otherwise return NULL.
tupleNode *findTupleNode(tupleList *clients, char *ipAddress, int portNumber)
{

    // If we are at the start of an empty tupleList.
    if (clients->headTupleNode == NULL)
    {

        return NULL;
    }
    else
    {

        // Keep the headTupleNode in a variable in order to go through all tha tupleNodes of the list without changing the headTupleNode.
        tupleNode *currentTuple = clients->headTupleNode;

        // Until there is an empty tupleNode.
        while (currentTuple != NULL)
        {

            // If there is another tupleNode with the same data.
            if (isTheSameTupleNode(currentTuple, ipAddress, portNumber))
            {

                return currentTuple;
            }
            else
            {

                // Move to the next tupleNode.
                currentTuple = currentTuple->nextTupleNode;
            }
        }

        // If we are here it means our tupleNode does not exist so we return NULL.
        return NULL;
    }
}
