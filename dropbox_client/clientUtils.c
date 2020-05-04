#include "clientUtils.h"

// Check if an IPv4 address is valid.
// Link with provided help: https://stackoverflow.com/questions/791982/determine-if-a-string-is-a-valid-ipv4-address-in-c
bool isValidIpAddress(char *ipAddress)
{
    // Variable for storing the ipAddress.
    struct sockaddr_in ip;

    // Check if the ipAddress is valid and get the returned value.
    int result = inet_pton(AF_INET, ipAddress, &(ip.sin_addr));

    // Return the result.
    return result != 0;
}

// Get the public ip of the client.
// Link with provided help: https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/.
char *getIpAddres()
{

    // The buffer we are going to use for our function.
    char helpBuffer[256];

    // Retrieve hostname.
    int hostname = gethostname(helpBuffer, sizeof(helpBuffer));
    if (hostname == -1)
    {
        printf("\n Error while getting host name! \n");

        // Return NULL in order to terminate the client.
        return NULL;
    }

    // To retrieve host information
    struct hostent *host = gethostbyname(helpBuffer);
    if (host == NULL)
    {

        printf("\n Error while getting host! \n");

        // Return NULL in order to terminate the client
        return NULL;
    }

    // Convert the ip address to the string we are going to return.
    char *ipAddress = inet_ntoa(*((struct in_addr *)host->h_addr_list[0]));
    return ipAddress;
}

// Send first the data length and then the data to the serverSocket.
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

// Receive first the sending data length and then the data from the server.
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

        printf("\n Received data: %s!\n", buffer);
    }

    // If we are here it means that everything completed successfully, so we are going to return the length of the received data.
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

// Client's thread which implements all its work flow.
// Link with provided help: https://gist.github.com/oleksiiBobko/43d33b3c25c03bcc9b2b
void *connectionHandler(void *Data)
{
    // Get the needed data for our thread.
    struct threadData *data = (struct threadData *)(Data);
    printf("\n Created thread %ld for client's connections manipulation! \n", pthread_self());

    // Get the socket descriptor of the new accepted client.
    int clientSocket = *(data->clientSocket);

    // Variable for the returned length of the received message
    int messageLength;

    // Buffers for receiving and sending data.
    char receiveBuffer[MAX_BUFFER_SIZE], sendBuffer[MAX_BUFFER_SIZE];

    // Lock the mutex.
    pthread_mutex_lock(&lock);

    // While we are receiving messages from the client.
    while ((messageLength = receiveData(clientSocket, receiveBuffer)) > 0 && shouldRun)
    {

        // Null terminate the received message.
        receiveBuffer[messageLength] = '\0';

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

        // Lock the mutex for the tupleList in order to be synchronized.
        pthread_mutex_lock(&dataLock);

        //If the sent message contains the USER_ON command we must add the new client to the tupleList.
        if (strstr(receiveBuffer, "USER_ON") != NULL && shouldRun)
        {

            // Add the new client to the tupleList.
            addToTupleList(data->clientsList, ipAddress, clientPort);
        }
        else if (strstr(receiveBuffer, "USER_OFF") != NULL && shouldRun)
        {

            // If the sent message contains the USER_OFF command we must remove the client from the tupleList.
            removeFromTupleList(data->clientsList, ipAddress, clientPort);
        }
        else if (strstr(receiveBuffer, "CLIENT_LIST") != NULL && shouldRun)
        {

            // If the sent message contains the CLIENT_LIST command we must add the connected clients to our tupleList.
            // Get all the message from the start until we reach the first space.
            char *tuples = strchr(receiveBuffer, ' ');

            // Get the tuples we are going to add but with one space at the start.
            char *tuplesToAdd = strchr(tuples + 2, ' ');

            // Remove the first space.
            tuplesToAdd += 1;

            // Then trim again until the first space in order to get the sent tuples number, convert it to number store it to the variable for iteration.
            tuples = strchr(tuples, ' ');
            int sentTuples = atoi(tuples);

            // Create a temporary tupleNode in order to compare it with the one we get as response from the server.
            tupleNode *tupleToCompare = createTupleNode(data->clientIpAddress, *(data->clientPort));

            // Loop through the sent message as many times as the sentTuples in order to add the connected clients to our list.
            for (int i = 0; i < sentTuples; i++)
            {

                // Do not add the node which is the current one.
                if (!isTheSameTupleNode(tupleToCompare, ipAddress, clientPort))
                {

                    // Add the client to the tupleList.
                    addToTupleList(data->clientsList, ipAddress, clientPort);
                }

                // As long as we have data.
                if (i < sentTuples - 1)
                {

                    // Go to the next space in order to get the next client.
                    // Get all the message until we reach the first space.
                    tuplesToAdd = strchr(tuplesToAdd, ' ');

                    // Get the tuples we are going to add but with one space at the start.
                    tuplesToAdd = strchr(tuplesToAdd + 2, ' ');

                    // Remove the first space.
                    tuplesToAdd += 1;

                    // Extract the wanted data from the receiverBuffer.
                    extractedIp = extractStringBetweenPatterns("<", ", ", tuplesToAdd);
                    extractedPort = extractStringBetweenPatterns(", ", ">", tuplesToAdd);

                    // Get the real ipAdress and port of the connected client.
                    clientPort = ntohs(atoi(extractedPort));

                    // For the ipAddress we should convert the hex string to uint32_t and then via in_addr struct and inet_ntoa convert it to the real one.
                    hexFromString = strtoul(extractedIp, NULL, 16);
                    tempIpAddress.s_addr = htonl(hexFromString);
                    ipAddress = inet_ntoa(tempIpAddress);

                    // Free the extracted char pointers.
                    free(extractedIp);
                    free(extractedPort);
                }
            }

            // Free the ipAddress of the client returned from createTupleNode() because we malloced in order to create enough space for the string.
            free(tupleToCompare->ipAddress);
            tupleToCompare->ipAddress = NULL;

            // Free the tupleNode we created at the start.
            free(tupleToCompare);
        }

        // Unlock mutex for the tupleList.
        pthread_mutex_unlock(&dataLock);

        // Initializing and clearing the buffers by setting enough space each time for every connected client.
        memset(receiveBuffer, 0, sizeof(receiveBuffer));
        memset(sendBuffer, 0, sizeof(sendBuffer));
    }

    // Unlock mutext for threads lock.
    pthread_mutex_unlock(&lock);

    return NULL;
}