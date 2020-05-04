#include "commandParser.h"

int commandParser(int argc, char *argv[], int *portNumber)
{

    // We check with 3 because the first argument holds the name of the program.
    if (argc == 3)
    {

        if (strcmp(argv[1], "-p") == 0)
        {

            // Check if the user input is a number.
            size_t idLength = strlen(argv[2]);

            for (int j = 0; j < idLength; j++)
            {

                if (!isdigit(argv[2][j]))
                {
                    printf("\n Please type a valid format for dropbox server's port and rerun the program! \n\n");

                    // Return 1 in order to know that we must exit program.
                    return 1;
                }
            }

            // Keep the id of the created client.
            *portNumber = atoi(argv[2]);
        }
        else
        {

            printf("\n Please type a valid format of command line arguments and rerun the program! \n\n");

            // Return 1 in order to know that we must exit program.
            return 1;
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