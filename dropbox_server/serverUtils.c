#include "serverUtils.h"

// Listening to port for connections and manipulate them.
// Links with provided help in which i did some parametrization:
// https://stackoverflow.com/questions/15560336/listen-to-multiple-ports-from-one-server
// https://www.binarytides.com/multiple-socket-connections-fdset-select-linux/
// This function created for listening to multiple ports but i erased that part and left the working code for one socket only.
int acceptConnectionRequest(int serverSocket, struct sockaddr *address, socklen_t *addressLength, int clientSockets[])
{

    // Variables for reading a request.
    // fd_set is a fixed size buffer for socket connections.
    fd_set readSockets;
    int maxFd;
    int socketDescriptor;
    int status;

    // Clear the fd_set.
    FD_ZERO(&readSockets);

    // Add the serverSocket to the fd_set.
    FD_SET(serverSocket, &readSockets);
    maxFd = serverSocket;

    // Add all clientSockets to the fd_set.
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
    {
        // Keep the socket descriptor.
        socketDescriptor = clientSockets[i];

        // If we have a valid socket descriptor then we must add it to the read fd_set.
        if (socketDescriptor > 0)
        {

            FD_SET(socketDescriptor, &readSockets);
        }

        // Keep the maximum descriptor in order to use it for select function.
        if (socketDescriptor > maxFd)
        {

            maxFd = socketDescriptor;
        }
    }

    // If we shouldRun select otherwise terminate
    if (!shouldRun)
    {

        // Return 0 in order to terminate.
        return 0;
    }

    // Wait for a new connection to select, timeout is NULL so this process will repeat forever.
    status = select(maxFd + 1, &readSockets, NULL, NULL, NULL);

    // If there was an error on select return -1.
    if ((status < 0) && (errno != EINTR))
    {

        printf("\n Error while accepting new client! \n");

        return -1;
    }

    // If there was a new connection in our serverSocket.
    if (FD_ISSET(serverSocket, &readSockets))
    {

        // If we shouldRun accept otherwise terminate
        if (!shouldRun)
        {

            // Return 0 in order to terminate.
            return 0;
        }

        // Return the accept function result in order to proceed in the main function.
        return accept(serverSocket, (struct sockaddr *)address, addressLength);
    }

    // If we are here it means that some other error occurred so we must return -1.
    printf("\n Error while accepting new client! \n");

    return -1;
}

// Receive first the sending data length and then the data from the client.
int receiveData(int socket, char *buffer)
{

    // Variables for counting the length of the received data and iterate over them.
    int receiveCounter = 0, length = 0;

    // First get the length of the send data.
    if (read(socket, &length, sizeof(length)) < 0)
    {

        // Return -1 in order to terminate the client
        return -1;
    }

    if (length > 0)
    {

        // Initialize the buffer for writing in it.
        memset(buffer, 0, MAX_BUFFER_SIZE);

        // Iterate over the data.
        for (int i = 0; i < length; i += receiveCounter)
        {

            if ((receiveCounter = read(socket, &buffer[i], length - i)) <= 0)
            {

                // Return -1 in order to terminate the client
                return -1;
            }
        }

        printf("\n Received data: %s! \n", buffer);
    }

    // If we are here it means that everything completed successfully, so we are going to return the length of the received data.
    return length;
}

// Send first the data length and then the data to the client.
int sendData(int serverSocket, char *data)
{

    // Get the length of the data which we are going to send. +1 is for the null terminator.
    int length = strlen(data) + 1;

    // First we are going to send the length of the data
    if (write(serverSocket, &length, sizeof(length)) < 0)
    {

        // Return -1 in order to terminate the client
        return -1;
    }

    // After sending the data length send the data.
    if (write(serverSocket, data, length) < 0)
    {

        // Return -1 in order to terminate the client
        return -1;
    }

    // If we are here it means that everything completed successfully, so we are going to return the length of the sent data.
    return length;
}

// Extract string between 2 patterns. Link with provided help: https://stackoverflow.com/questions/24696113/how-to-find-text-between-two-strings-in-c
char *extractStringBetweenPatterns(char *startPattern, char *endPattern, char *stringToExtractFrom)
{

    // Variables for keeping target, start and end string.
    char *target = NULL, *start, *end;

    // Get as start string the startPattern from the stringToExtractFrom.
    start = strstr(stringToExtractFrom, startPattern);

    // If start exists proceed.
    if (start)
    {

        // Move as many characters as the pattern in order to be at the start of the target string.
        start += strlen(startPattern);

        // Get as end string the endPattern from the stringToExtractFrom.
        end = strstr(start, endPattern);

        // If end exists proceed.
        if (end)
        {

            // Get enough space for out target string.
            target = malloc(end - start + 1);

            // Copy the string we want to extract to our target and then null terminate it.
            memcpy(target, start, end - start);
            target[end - start] = '\0';
        }
    }

    // If the target found return it and do not forget to free it later.
    if (target)
    {

        return target;
    }

    // If we are here it means that our target does not exist so we return null.
    return 0;
}

// Server's thread which implements all its work flow.
// Link with provided help: https://gist.github.com/oleksiiBobko/43d33b3c25c03bcc9b2b
void *connectionHandler(void *Data)
{

    // Get the needed data for our thread.
    struct threadData *data = (struct threadData *)(Data);
    printf("\n Created thread %ld for server's connections manipulation! \n", pthread_self());

    // Increase the threadCounter.
    threadCounter++;

    // Get the socket descriptor of the new accepted client.
    int acceptedClient = *(data->acceptedClient);

    // Variable for the returned length of the received message
    int messageLength;

    // Buffers for receiving and sending data.
    char receiveBuffer[MAX_BUFFER_SIZE], sendBuffer[MAX_BUFFER_SIZE];

    // Initializing and clearing the buffers by setting enough space each time for every connected client.
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
    memset(sendBuffer, 0, sizeof(sendBuffer));

    // While we are receiving messages from the client.
    while ((messageLength = receiveData(acceptedClient, receiveBuffer)) > 0 && shouldRun)
    {
        // Null terminate the received message.
        receiveBuffer[messageLength] = '\0';

        // If the clients sent something.
        if (messageLength > 0 && shouldRun)
        {

            // Extract the wanted data from the receiverBuffer.
            char *extractedIp = extractStringBetweenPatterns("<", ", ", receiveBuffer);
            char *extractedPort = extractStringBetweenPatterns(", ", ">", receiveBuffer);

            // Get the real ipAdress and port of the connected client.
            int clientPort = ntohs(atoi(extractedPort));

            // For the ipAddress we should convert the hex string to uint32_t and then via in_addr struct and inet_ntoa convert it to the real one.
            uint32_t hexFromString = strtoul(extractedIp, NULL, 16);
            struct in_addr tempIpAddress;
            tempIpAddress.s_addr = htonl(hexFromString);
            char *ipAddress = inet_ntoa(tempIpAddress);

            // Free the extracted char pointers.
            free(extractedIp);
            free(extractedPort);

            // Reinitialize the socketDescriptor everytime for the new connection.
            acceptedClient = *(data->acceptedClient);

            //If the sent message contains the LOG_ON command we must add the new client to the tupleList and inform all the existed clients for it.
            if (strstr(receiveBuffer, "LOG_ON") != NULL && shouldRun)
            {

                // Add the new client to the tupleList.
                int addResult = addToTupleList(data->clientsList, ipAddress, clientPort, acceptedClient);

                // If the client did not exist in our clientsList then we must inform every client about his appearance.
                if (addResult != -1 && shouldRun)
                {

                    // Send to every existed client inside our tupleList.
                    // If there are any clients inside our list.
                    if (data->clientsList != NULL && shouldRun)
                    {
                        // Send the USER_ON <tuple> message to all the clients.
                        printf("\n Sending USER_ON tuple to all the already connected clients ... \n");

                        // Convert ip and port before sending them to binary via sprintf().
                        snprintf(sendBuffer, sizeof sendBuffer, "%s <%x, %d>", "USER_ON", ntohl(tempIpAddress.s_addr), ntohs(clientPort));

                        // Pointer for keeping the headTupleNode in order to go through all the tupleNodes list without changing the head.
                        tupleNode *iteratableTupleNode = data->clientsList->headTupleNode;

                        // While we have not reach the end of our list.
                        while (iteratableTupleNode != NULL && shouldRun)
                        {
                            // Do not send the message to the current connected client.
                            if (!isTheSameTupleNode(iteratableTupleNode, ipAddress, clientPort) && shouldRun)
                            {
                                printf("\n Sending USER_ON to %s at port %d\n", iteratableTupleNode->ipAddress, iteratableTupleNode->portNumber);

                                // Sockets are bidirectional so if the clients exist in our list we can just use their socket descriptor to send to them.
                                sendData(iteratableTupleNode->fdSocketClient, sendBuffer);
                            }

                            // Go to the next tupleNode of the tupleList.
                            iteratableTupleNode = iteratableTupleNode->nextTupleNode;
                        }
                    }
                }
            }
            else if (strstr(receiveBuffer, "LOG_OFF") != NULL && shouldRun)
            {

                // Convert ip and port before sending them to binary via sprintf().
                snprintf(sendBuffer, sizeof sendBuffer, "%s <%x, %d>", "USER_OFF", ntohl(tempIpAddress.s_addr), ntohs(clientPort));

                // Pointer for keeping the headTupleNode in order to go through all the tupleNodes list without changing the head.
                tupleNode *iteratableTupleNode = data->clientsList->headTupleNode;

                // While we have not reach the end of our list.
                while (iteratableTupleNode != NULL && shouldRun)
                {

                    // Do not send the message to the current connected client.
                    if (!isTheSameTupleNode(iteratableTupleNode, ipAddress, clientPort) && shouldRun)
                    {
                        printf("\n Sending USER_OFF to %s at port %d\n", iteratableTupleNode->ipAddress, iteratableTupleNode->portNumber);

                        // Sockets are bidirectional so if the clients exist in our list we can just use their socket descriptor to send to them.
                        sendData(iteratableTupleNode->fdSocketClient, sendBuffer);
                    }

                    // Go to the next tupleNode of the tupleList.
                    iteratableTupleNode = iteratableTupleNode->nextTupleNode;
                }

                // If the sent message contains the LOG_OFF command we must remove the client from the tupleList and inform all the existed clients for it.
                tupleNode *tupleToBeDeleted = findTupleNode(data->clientsList, ipAddress, clientPort);

                // Close the socket with this client.
                close(tupleToBeDeleted->fdSocketClient);

                // Remove the client from the clientsList.
                removeFromTupleList(data->clientsList, ipAddress, clientPort);

                return NULL;
            }
            else if (strstr(receiveBuffer, "GET_CLIENTS") != NULL && shouldRun)
            {
                //If the sent message contains the GET_CLIENTS command we must send the list of the existed clients to the one who requested.
                printf("\n Sending GET_CLIENTS tuple to the connected client ... \n");

                // Convert ip and port before sending them to binary via sprintf().
                snprintf(sendBuffer, sizeof sendBuffer, "%s %d", "CLIENT_LIST", data->clientsList->size);

                // Pointer for keeping the headTupleNode in order to go through all the tupleNodes list without changing the head.
                tupleNode *iteratableTupleNode = data->clientsList->headTupleNode;

                // Temporary struct in order to save the public ipAddress and convert it to binary.
                struct in_addr tempIpAddress;

                // While we have not reach the end of our list.
                while (iteratableTupleNode != NULL && shouldRun)
                {

                    // Convert ip to binary for send process.
                    inet_aton(iteratableTupleNode->ipAddress, &tempIpAddress);

                    // Append the tuple to the sendBuffer.
                    snprintf(sendBuffer + strlen(sendBuffer), sizeof sendBuffer, " <%x, %d>", ntohl(tempIpAddress.s_addr), ntohs(iteratableTupleNode->portNumber));

                    // Go to the next tupleNode of the tupleList.
                    iteratableTupleNode = iteratableTupleNode->nextTupleNode;
                }

                sendData(acceptedClient, sendBuffer);
            }

            // Initializing and clearing the buffers by setting enough space each time for every connected client.
            memset(receiveBuffer, 0, sizeof(receiveBuffer));
            memset(sendBuffer, 0, sizeof(sendBuffer));
        }
    }

    return NULL;
}
