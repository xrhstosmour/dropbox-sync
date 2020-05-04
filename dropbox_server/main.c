#include "serverUtils.h"

// Global variables because of signal handler which can not accept arguments.
// Variable in order to know how many threads have been created.
int volatile threadCounter = 0;

// Variable in order to know when we should stop the program execution.
bool volatile shouldRun = 1;

int main(int argc, char *argv[])
{

    // Command line variable.
    int portNumber = 0;

    // Needed variables for creating the server and reading the client's info and requests.
    int clientSockets[MAX_CLIENTS_SIZE];
    struct sockaddr_in server, client;
    struct sockaddr *serverPointer = (struct sockaddr *)&server, *clientPointer = (struct sockaddr *)&client;
    socklen_t clientSize = sizeof(clientPointer);
    struct hostent *clientInfo;
    int acceptedClient;

    // Create an array of threads for our connection manipulation as many as the maximum clients which can connect to the server.
    pthread_t serverThreads[MAX_CLIENTS_SIZE];

    // The server socket.
    int serverSocket = 0;

    // The tuple list for our connected clients.
    tupleList *clientsList = NULL;

    // The structure for the data we are passing through our thread.
    struct threadData *data = NULL;

    // Variable in order to manipulate signalHandler because of fgets and read in threads.
    struct sigaction signalAction;

    printf("\n Program execution started ... \n");

    // Check the command line arguments before proceeding.
    int error = commandParser(argc, argv, &portNumber);

    if (error == 1)
    {

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
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

    // Create the tupleList.
    clientsList = createTupleList();
    if (clientsList == NULL)
    {

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
    }
    else
    {

        printf("\n TupleList created! \n");
    }

    // Initialise all clientsSockets to 0 so as not to be checked.
    for (int i = 0; i < MAX_CLIENTS_SIZE; i++)
    {

        clientSockets[i] = 0;
    }

    // Create the socket for communicating with the server.
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {

        printf("\n Error while creating the serverSocket! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
    }
    else
    {

        printf("\n Server socket has been created! \n");
    }

    // Setting options for the serverSocket in order to accept multiple connections.
    int options = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options)) == -1)
    {

        printf("\n Error while setting options for the serverSocket! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
    }

    // Setting internet domain, address and port for the server.
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(portNumber);

    // Binding serverSocket to the address.
    if (bind(serverSocket, serverPointer, sizeof(server)) < 0)
    {

        printf("\n Error while binding serverSocket to the address! \n");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
    }

    // Initializing listening for connections by adding maximum of pending connections.
    if (listen(serverSocket, MAX_PENDING_LISTEN_SIZE) < 0)
    {

        printf("Error while initializing listening for serverSocket!");

        // Terminate program by freeing everything.
        exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, NULL);
    }

    printf("\n Listening for connections to port %d ... \n", portNumber);

    // Data structure which contains every needed variable for our thread work flow.
    data = malloc(sizeof(struct threadData));

    // Constantly listening for new connections until someone press CTRL + C.
    // Using acceptConnectionRequest function and select in order to accept connections.
    while ((acceptedClient = acceptConnectionRequest(serverSocket, clientPointer, &clientSize, clientSockets)) && shouldRun)
    {

        // Get the connected client's info.
        if ((clientInfo = gethostbyaddr((char *)&client.sin_addr.s_addr, sizeof(client.sin_addr.s_addr), client.sin_family)) == NULL)
        {

            printf("\n Error while getting connected client's info! \n");

            // Terminate program by freeing everything.
            exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, serverThreads);
        }

        // Get the ip of the connected client.
        char connectedIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(client.sin_addr), connectedIp, INET_ADDRSTRLEN);

        printf("\n Client with ip address %s and port %d connected! \n", connectedIp, ntohs(client.sin_port));

        // Pass the needed data which is our clientList, the serverSocket and the connectedIp.
        data->clientsList = clientsList;
        data->acceptedClient = &acceptedClient;
        data->serverSocket = &serverSocket;

        // Create the thread for our connection manipulation.
        if (pthread_create(&(serverThreads[threadCounter]), NULL, connectionHandler, (void *)data) < 0 && threadCounter > MAX_CLIENTS_SIZE)
        {
            printf("\n Error while creating the server thread! \n");

            // Terminate program by freeing everything.
            exitProgram(EXIT_FAILURE, serverSocket, clientsList, data, serverThreads);
        }
    }

    // Terminate program by freeing everything.
    exitProgram(EXIT_SUCCESS, serverSocket, clientsList, data, serverThreads);

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
void exitProgram(int status, int serverSocket, tupleList *clientsList, struct threadData *data, pthread_t serverThreads[])
{

    printf("\n Program will terminate! \n\n");

    // Change shouldRun variable to 0 in order to terminate every loop and thread.
    shouldRun = 0;

    // If the clientsList is not NULL we should destroy her.
    if (clientsList != NULL)
    {
        // Destroy the tupleList with all the clients.
        destroyTupleList(clientsList);
    }

    // Free the data structure we pass to each thread.
    free(data);

    // Wait for thread termination.
    if (serverThreads != NULL)
    {

        // Wait for every thread to complete.
        for (int i = 0; i < threadCounter; i++)
        {

            pthread_join(serverThreads[i], NULL);
        }
    }

    // Close the serverSocket before terminating.
    close(serverSocket);

    exit(status);
}
