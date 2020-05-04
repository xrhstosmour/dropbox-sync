#include "commandParser.h"

// Match the command line arguments to the specific variables and check them. Return 1 if there was an error and so we must exit program.
int commandParser(int argc, char *argv[], DIR **directoryDIR, char **directoryName, int *portNumber, int *workerThreads,
                  int *bufferSize, int *serverPort, char **serverIp)
{

    // We check with 13 because the first argument holds the name of the program.
    if (argc == 13)
    {

        for (int i = 1; i < 13; i += 2)
        {

            if (strcmp(argv[i], "-d") == 0)
            {

                // Initialize stat variable in order to check if the  directory exists.
                struct stat directory = {0};

                // If the common directory does not exist we must terminate the program.
                if (stat(argv[i + 1], &directory) == -1)
                {

                    printf("\n The given directory does not exist! \n\n");

                    // Return 1 in order to know that we must exit program.
                    return 1;
                }

                // Open the common directory in order to get access to the files of each client.
                *directoryDIR = opendir(argv[i + 1]);
                if (*directoryDIR == NULL)
                {

                    printf("\n There was an error while opening common directory! \n\n");

                    // Return 1 in order to know that we must exit program.
                    return 1;
                }

                // Pass the directory path to the variable.
                *directoryName = strdup(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-p") == 0)
            {

                // Check if the user input is a number.
                size_t portNumberLength = strlen(argv[i + 1]);
                for (int j = 0; j < portNumberLength; j++)
                {

                    if (!isdigit(argv[i + 1][j]))
                    {
                        printf("\n Please type a valid format for port's number and rerun the program! \n\n");

                        // Return 1 in order to know that we must exit program.
                        return 1;
                    }
                }

                // Keep the portNumber.
                *portNumber = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-w") == 0)
            {

                // Check if the user input is a number.
                size_t workerThreadsLength = strlen(argv[i + 1]);
                for (int j = 0; j < workerThreadsLength; j++)
                {

                    if (!isdigit(argv[i + 1][j]))
                    {
                        printf("\n Please type a valid format for worker thread's number and rerun the program! \n\n");

                        // Return 1 in order to know that we must exit program.
                        return 1;
                    }
                }

                // Keep the workerThreads.
                *workerThreads = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-b") == 0)
            {

                // Check if the user input is a number.
                size_t bufferLength = strlen(argv[i + 1]);
                for (int j = 0; j < bufferLength; j++)
                {

                    if (!isdigit(argv[i + 1][j]))
                    {
                        printf("\n Please type a valid format for buffer's size and rerun the program! \n\n");

                        // Return 1 in order to know that we must exit program.
                        return 1;
                    }
                }

                // Keep the size of the buffer.
                *bufferSize = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-sp") == 0)
            {

                // Check if the user input is a number.
                size_t serverPortLength = strlen(argv[i + 1]);
                for (int j = 0; j < serverPortLength; j++)
                {

                    if (!isdigit(argv[i + 1][j]))
                    {
                        printf("\n Please type a valid format for server's port and rerun the program! \n\n");

                        // Return 1 in order to know that we must exit program.
                        return 1;
                    }
                }

                // Keep the serverPort.
                *serverPort = atoi(argv[i + 1]);
            }
            else if (strcmp(argv[i], "-sip") == 0)
            {

                // Check if the serverIp is valid.
                if (!isValidIpAddress(argv[i + 1]))
                {

                    printf("\n Please type a valid format for server's Ip address and rerun the program! \n\n");

                    // Return 1 in order to know that we must exit program.
                    return 1;
                }

                // Pass  the Ip of to the serverPort variable.
                *serverIp = strdup(argv[i + 1]);
            }
            else
            {

                printf("\n Please type a valid format of command line arguments and rerun the program! \n\n");

                // Return 1 in order to know that we must exit program.
                return 1;
            }
        }
    }
    else
    {

        printf("\n Please type a valid format of command line arguments and rerun the program! \n\n");

        // Return 1 in order to know that we must exit program.
        return 1;
    }

    // If we reach that point it means that everything executed fine.
    return 0;
}