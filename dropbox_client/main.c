#include "clientUtils.h"

// Global variables because of signal handler which can not accept arguments.
// Variable in order to know when we should stop the program execution.
bool volatile shouldRun = 1;

// Mutex lock and dataLock for preventing the threads from multiple same executions.
pthread_mutex_t lock, dataLock;

int main(int argc, char *argv[])
{

    // Command line variables.
    int portNumber = 0, bufferSize = 0, serverPort = 0, workerThreads = 0;

    // Directory pointer in order to access the folder we are going to use for synchronization.
    DIR *directoryDIR = NULL;

    // Char pointer for each path which we are going to use for storing files, directories and data.
    char *directoryName = NULL;

    // Char pointer for server's and client's ip.
    char *serverIp = NULL, *clientIpAddress = NULL;

    // Needed variables for creating the clientForServer, find the server, reading and writing requests.
    int clientSocket = 0;
    struct sockaddr_in clientForServer, client;
    struct sockaddr *clientPointerForServer = (struct sockaddr *)&clientForServer, *clientPointer = (struct sockaddr *)&client;
    struct hostent *serverInfo;

    // Pointer to our tupleList with all the clients.
    tupleList *clientsList = NULL;

    // Pointer to our threadData which we are going to pass to the created threads.
    struct threadData *data = NULL;

    // Buffers for sending and receiving data.
    char receiveBuffer[MAX_BUFFER_SIZE], sendBuffer[MAX_BUFFER_SIZE];

    // Variable in order to manipulate signalHandler because of fgets and read in threads.
    struct sigaction signalAction;

    printf("\n Program execution started ... \n");

    // Check the command line arguments before proceeding.
    int error = commandParser(argc, argv, &directoryDIR, &directoryName, &portNumber, &workerThreads, &bufferSize,
                              &serverPort, &serverIp);

    if (error == 1)
    {

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Catch if the user send any signal via signalAction and signalHandler function to terminate program execution.
    // Link with provided help: https://stackoverflow.com/questions/38959797/implement-ctr-c-using-sigaction.
    // Initialize the signalAction variable with our signalHandler function.
    signalAction.sa_handler = signalHandler;
    sigemptyset(&signalAction.sa_mask);

    // Signal restart (SA_RESTART)
    signalAction.sa_flags = 0;

    // Manipulate signalHandler for SIGINT signal.
    sigaction(SIGINT, &signalAction, NULL);

    // Initialize the lock mutex to avoid multiple threads on the same connectionHandler.
    if (pthread_mutex_init(&lock, NULL) != 0)
    {

        printf("\n There was an error while initializing lock mutex! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Initialize the dataLock mutex to avoid multiple threads on the same connectionHandler.
    if (pthread_mutex_init(&dataLock, NULL) != 0)
    {

        printf("\n There was an error while initializing dataLock mutex! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Create the socket for communicating with the clientForServer.
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {

        printf("\n Error while creating the clientSocket! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }
    else
    {

        printf("\n Client socket has been created! \n");
    }

    // Setting options for the clientSocket in order to accept multiple connections.
    int options = 1;
    if (setsockopt(clientSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options)) == -1)
    {

        printf("\n Error while setting options for the clientSocket! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Find the server address.
    if ((serverInfo = gethostbyname(serverIp)) == NULL)
    {

        printf("\n Error while searching for the server! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Setting internet domain, address and port for the client connection to server.
    clientForServer.sin_family = AF_INET;
    memcpy(&clientForServer.sin_addr, serverInfo->h_addr, serverInfo->h_length);
    clientForServer.sin_port = htons(serverPort);

    // Setting internet domain, address and port for the client.
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = htonl(INADDR_ANY);
    client.sin_port = htons(portNumber);

    // Binding clientSocket to the address.
    if (bind(clientSocket, clientPointer, sizeof(client)) < 0)
    {

        printf("\n Error while binding clientSocket to the address! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    // Initializing connecting procedure.
    if (connect(clientSocket, clientPointerForServer, sizeof(clientForServer)) < 0)
    {

        printf("\n Error while initializing connection with serverSocket! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }

    printf("\n Connecting to %s at port %d\n", serverInfo->h_name, serverPort);

    // Create the tupleList.
    clientsList = createTupleList();
    if (clientsList == NULL)
    {

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, NULL, workerThreads);
    }
    else
    {

        printf("\n TupleList created! \n");
    }

    // Create the workerThreads for our incoming messages manipulation.
    pthread_t clientThreads[workerThreads];

    // Initializing by setting enough space each time for every connection.
    memset(receiveBuffer, 0, sizeof(receiveBuffer));
    memset(sendBuffer, 0, sizeof(sendBuffer));

    // After a successful connection we must send the LOG_ON message to the server.
    printf("\n Sending LOG_ON tuple to the server... \n");

    // First we need to get the public ip address of the client.
    clientIpAddress = strdup(getIpAddres());
    if (clientIpAddress == NULL)
    {

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, clientThreads, workerThreads);
    }

    // Temporary struct in order to save the public ipAddress and convert it to binary.
    struct in_addr tempIpAddress;
    inet_aton(clientIpAddress, &tempIpAddress);

    // Data structure which contains every needed variable for our thread work flow.
    data = malloc(sizeof(struct threadData));

    // Pass the needed data which is our clientList, the clientSocket.
    data->clientsList = clientsList;
    data->clientSocket = &clientSocket;
    data->clientIpAddress = clientIpAddress;
    data->clientPort = &portNumber;

    // Convert ip and port before sending them to binary via sprintf().
    snprintf(sendBuffer, sizeof sendBuffer, "%s <%x, %d>", "LOG_ON", ntohl(tempIpAddress.s_addr), ntohs(portNumber));

    // Send the data to the server.
    sendData(clientSocket, sendBuffer);

    // Send the command for getting all the connected clients in the server.
    printf("\n Sending GET_CLIENTS tuple to the server... \n");
    snprintf(sendBuffer, sizeof sendBuffer, "%s <%x, %d>", "GET_CLIENTS", ntohl(tempIpAddress.s_addr), ntohs(portNumber));

    // The same must be done for any other movement.
    for (int i = 0; i < workerThreads; i++)
    {
        // Create the thread for our connection manipulation.
        if (pthread_create(&(clientThreads[i]), NULL, connectionHandler, (void *)data) < 0)
        {
            printf("\n Error while creating the server thread! \n");

            // Terminate program by freeing everything.
            exitProgram(EXIT_FAILURE, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, clientThreads, workerThreads);
        }
    }

    // Send the data to the server.
    sendData(clientSocket, sendBuffer);

    while (shouldRun)
    {

        printf("\n Type the number of the command you want to execute: \n");
        printf("\n 1. GET_FILE_LIST \n");
        printf("\n 2. GET_FILE <pathname, version> \n");
        printf("\n 3. LOG_OFF <IP, port> \n");

        // Get the use command in order to execute it.
        fgets(receiveBuffer, sizeof(receiveBuffer), stdin);

        // Null terminate the use command inside the buffer.
        receiveBuffer[strlen(receiveBuffer) - 1] = '\0';

        if (strcmp(receiveBuffer, "1") == 0)
        {

            printf("\n Not implemented function! \n");
        }
        else if (strcmp(receiveBuffer, "2") == 0)
        {

            printf("\n Not implemented function! \n");
        }
        else if (strcmp(receiveBuffer, "3") == 0)
        {

            // Terminate program by freeing everything.
            exitProgram(EXIT_SUCCESS, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, clientThreads, workerThreads);
        }
        else
        {
            if (shouldRun)
            {
                printf("\n Please type an accepted number from the above list! \n");
            }
        }
    }

    // Terminate program by freeing everything.
    exitProgram(EXIT_SUCCESS, clientSocket, &directoryDIR, directoryName, serverIp, clientIpAddress, clientsList, data, clientThreads, workerThreads);

    return 0;
}

// Terminate the program by breaking the infinite loop.
void signalHandler(int signal)
{

    // Terminate the program if the user typed CTRL + C.
    if (signal == SIGINT)
    {

        // Change shouldRun variable to 0 in order to terminate every loop and thread.
        shouldRun = 0;
    }
}

// Terminate the program after freeing everything.
void exitProgram(int status, int clientSocket, DIR **directoryDIR, char *directoryName, char *serverIp, char *clientIpAddress, tupleList *clientsList, struct threadData *data, pthread_t clientThreads[], int workerThreads)
{

    // Buffers for sending the LOG_OFF signal to the server.
    char sendBuffer[MAX_BUFFER_SIZE];

    // Change shouldRun variable to 0 in order to terminate every loop and thread.
    shouldRun = 0;

    // If we have send anything to our server.
    if (data != NULL)
    {
        // Temporary struct in order to save the public ipAddress and convert it to binary.
        struct in_addr logOffIpAddress;
        inet_aton(clientIpAddress, &logOffIpAddress);

        // Send the LOG_OFF <tuple> message to the server.
        printf("\n Sending LOG_OFF tuple to the server... \n");

        // Convert ip and port before sending them to binary via sprintf().
        snprintf(sendBuffer, sizeof sendBuffer, "%s <%x, %d>", "LOG_OFF", ntohl(logOffIpAddress.s_addr), ntohs(*(data->clientPort)));

        // Send the data to the server.
        sendData(clientSocket, sendBuffer);
    }

    // Free the directoryDIR and the directoryName pointer if exists.
    if (*directoryDIR != NULL)
    {

        closedir(*directoryDIR);
    }

    if (directoryName != NULL)
    {

        free(directoryName);
    }

    // If the serverIp char pointer exists we should free it.
    if (serverIp != NULL)
    {

        free(serverIp);
    }

    // If the clientIpAddress char pointer exists we should free it.
    if (clientIpAddress != NULL)
    {

        free(clientIpAddress);
    }

    // If the clientsList is not NULL we should destroy her.
    if (clientsList != NULL)
    {
        // Destroy the tupleList with all the clients.
        destroyTupleList(clientsList);
    }

    // Wait for thread termination.
    if (clientThreads != NULL)
    {

        // Wait for every thread to complete.
        for (int i = 0; i < workerThreads; i++)
        {
            pthread_join(clientThreads[i], NULL);
        }
    }

    // Destroy the lock, dataLock mutex.
    pthread_mutex_destroy(&lock);
    pthread_mutex_destroy(&dataLock);

    // If the dataThread structure is not NULL we must free it.
    if (data != NULL)
    {

        free(data);
    }

    // Close the serverSocket before terminating.
    close(clientSocket);

    printf("\n Program will terminate! \n\n");

    exit(status);
}
