/* johnwalt.adventure.c */

// Walter Johnson: johnwalt@oregonstate.edu
// CS344 OPERATING SYSTEMS I
// Winter 2020
// Program 2

// Based on code written by Walter for CS344 Fall 2019 with permission from Bram Lewis.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>
#include <pthread.h>

pthread_mutex_t gameMutex = PTHREAD_MUTEX_INITIALIZER;

typedef enum { false, true } bool; // from: https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

struct room // struct used in global array of rooms
{
    char name[12];
    char type[12];
    char connections[6][12];
};

struct room rooms[7]; // global array of room structs

/*--Function Prototypes---------------------------------------------------------------------------*/
void GetRoomsDirectory(char* dirName);
        // fills char array with name of most recent rooms directory
void GetRoomNames(char* dirName, char** roomsArray);
        // fills 2D array with names of seven rooms
void FillArrayOfRoomStructs(char* dirName, char** roomsArray);
        // fills array of room structs called rooms
void PrintCurrentLocation(int currentLocation);
        // prints current location to stdout
void GetAndPrintConnections(int currentLocation);
        // gets from file and prints possible connections
void SolicitUser(char* userInput);
        // prints "WHERE TO? >" and takes user input
bool IsValidConnection(int currentLocation, char* userInput);
        // returns true if user input is the name of a room that is connected to the current room
bool IsTimeCommand(char* userInput);
        // returns true if user input is the time command
void GetFormattedTimeString(char* currTime);
        // returns properly formatted string of current time: " 1:03pm, Tuesday, September 13, 2016"
void* CreateAndWriteTimeFile(void* argument);
        // Creates time file and writes current time to file
void ReadTimeFile(char* currTime);
        // reads time file and gives time string to pointer
int FindNextCurrentLocation(char* userInput);
        // takes validated user input and returns index of next current location
void RoomNotValid();
        // prints "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN." if user input is not valid
bool IsEndRoom(int currentLocation);
        // returns true if room is end room
/*---------------------------------------------------------------------------Function Prototypes--*/

int main()
{
    pthread_t timeThread;
    pthread_mutex_lock( &gameMutex ); // lock out soom to be created second thread

    // second thread ready to write formatted time to "currentTime.txt"
    int result = pthread_create( &timeThread, NULL,  CreateAndWriteTimeFile, NULL );

    int i = -7;
    // int j = -6;

    char *dirName = (char *) malloc(sizeof(char) * 32); // name of directory of room files
    memset(dirName, '\0', sizeof(char) * 32);
    GetRoomsDirectory(dirName);

    char **roomsArray = (char**) malloc(sizeof(char*) * 7); // to be returned
    for (i = 0; i < 7; i++)
    {
        roomsArray[i] = (char*) malloc(sizeof(char) * 16);
        memset(roomsArray[i], '\0', sizeof(char) * 16);
    }
    GetRoomNames(dirName, roomsArray);

    memset(rooms, '\0', sizeof(rooms));
    FillArrayOfRoomStructs(dirName, roomsArray); // all the info needed for game

    int currentLocation = 0;
    char *userInput = (char *) malloc(sizeof(char) * 128);
    memset(userInput, '\0', sizeof(char) * 128);
    bool isEndRoom = false;
    int steps = 0;       // counts steps
    int roomIndexes[64]; // saves indexes of rooms used enroute to end

    while (isEndRoom == false)
    {
        bool isTimeCommand = false;
        PrintCurrentLocation(currentLocation);
        GetAndPrintConnections(currentLocation);
        do // repeats just this part of loop if user calls time
        {
            isTimeCommand = false;
            SolicitUser(userInput);
            bool isValidConnection = IsValidConnection(currentLocation, userInput);
            if (isValidConnection == true) // move to next room
            {
                currentLocation = FindNextCurrentLocation(userInput);
                roomIndexes[steps] = currentLocation;
                steps++;
            }
            else if (IsTimeCommand(userInput) == true)
            {
                pthread_mutex_unlock( &gameMutex ); // let the second thread activate
                result = pthread_join(timeThread, NULL);
                    // CreateAndWriteTimeFile(); runs as separate thread now
                pthread_mutex_lock( &gameMutex );
                result = pthread_create( &timeThread, NULL,  CreateAndWriteTimeFile, NULL ); // recreate second thread

                char *currTime = (char *) malloc(sizeof(char) * 42);
                memset(currTime, '\0', sizeof(char) * 42);
                ReadTimeFile(currTime);
                printf("\n%s\n", currTime);
                isTimeCommand = true;
                free(currTime);
            }
            else
            {
                RoomNotValid();
            }
            isEndRoom = IsEndRoom(currentLocation);
        } while (isTimeCommand == true);
    }

    printf("\nYOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
    char S[] = "S";
    if (steps == 1)
    {
        strcpy(S, "");
    }
    printf("YOU TOOK %d STEP%s. YOUR PATH TO VICTORY WAS:\n", steps, S);
    for (i = 0; i < steps; i++)
    {
        printf("%s\n", rooms[roomIndexes[i]].name);
    }

    free(dirName);
    for (i = 0; i < 7; i++)
    {
        free(roomsArray[i]);
    }
    free(roomsArray);
    free(userInput);

    return 0;
}

// takes pointer to char array, fills with name of most recent rooms directory
    // from reading 2.4 Manipulating Directories
void GetRoomsDirectory(char* dirName)
{
    int newestDirTime = -1;
    char targetDirPrefix[32] = "johnwalt.rooms.";
    char newestDirName[32];
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR* dirToCheck;
    struct dirent *fileInDir; 
    struct stat dirAttributes;

    dirToCheck = opendir("."); // open current directory

    if (dirToCheck > 0)
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL)
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
            {
                stat(fileInDir->d_name, &dirAttributes);

                if ((int)dirAttributes.st_mtime > newestDirTime) // if latest directory checked is newest yet
                {
                    newestDirTime = (int)dirAttributes.st_mtime;
                    memset(newestDirName, '\0', sizeof(newestDirName));
                    strcpy(newestDirName, fileInDir->d_name);
                }
            }
        }
    }

    closedir(dirToCheck);

    strcpy(dirName, "./");
    strcat(dirName, newestDirName); // gives us: "./johnwalt.room.<PID>"
}

// takes pointer to name of directory and pointer to 2D char array, fills 2D array with names of seven rooms
void GetRoomNames(char* dirName, char** roomsArray)
{
    int i = 0;
    DIR* roomDir;
    struct dirent *fileInDir;
    roomDir = opendir(dirName);
    if (roomDir > 0)
    {
        while ((fileInDir = readdir(roomDir)) != NULL)
        {
            if (fileInDir->d_type == DT_REG && i < 7) // regular files only, assumes only seven
            {
                strcpy(roomsArray[i], strtok(fileInDir->d_name, "_"));
                i++;
            }
        }
    }
    
    closedir(roomDir);
}

// takes name of room directory and array of room names, fills array of room structs called rooms
void FillArrayOfRoomStructs(char* dirName, char** roomsArray)
{
    int i = -7;
    int j = -8;
    int file_descriptor;
    char filePath[48]; // path to each room file
    char readBuffer[256]; // temp storage for contents of each room file
    size_t nread;
    const char newline[3] = "\n"; // delimiter for strtok()
    const char colon[3] = ":"; // delimiter for strtok()
    char *tempString;
    char tempArray[8][24]; // array of lines from room files
    char splitArray[8][10]; // array of needed text from room files
    int midRoom = 0;

    for (i = 0; i < 7; i++) // iterate through roomsArray
    {
        for (j = 0; j < 8; j++)
        {
            memset(tempArray[j], '\0', sizeof(tempArray[j])); 
            memset(splitArray[j], '\0', sizeof(splitArray[j]));
        }
        file_descriptor = -1;
        memset(filePath, '\0', sizeof(filePath));
        strcpy(filePath, dirName);
        strcat(filePath, "/");
        strcat(filePath, roomsArray[i]);
        strcat(filePath, "_room"); // filePath to each of the room files

        file_descriptor = open(filePath, O_RDONLY);

        if (file_descriptor == -1)
        {
            printf("open() failed on \"%s\"\n", filePath);
        }

        memset(readBuffer, '\0', sizeof(readBuffer));
        nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

        tempString = strtok(readBuffer, newline); // first line of room file
        int k = 0;  // keeps track of number of lines in room file
        strcpy(tempArray[k], tempString);

        while (tempString != NULL)
        {
            strcpy(tempArray[k], tempString);
            tempString = strtok(NULL, newline); // following lines of room file
            k++;
        }

        for (j = 0; j < k; j++) // iterate through array of room file lines
        {
            strtok(tempArray[j], colon);
            tempString = strtok(NULL, colon);  // takes string after colon, but includes leading space
            sscanf(tempString, "%s", splitArray[j]); // gets rid of leading space
        }

        int roomsIndex = 0; 
        if (strcmp(splitArray[k - 1], "START_ROOM") == 0)
        {
            roomsIndex = 0; // places start room at index 0
        }
        else if (strcmp(splitArray[k - 1], "MID_ROOM") == 0)
        {
            midRoom++;
            roomsIndex = midRoom; // up to five mid rooms
        }
        else if (strcmp(splitArray[k - 1], "END_ROOM") == 0)
        {
            roomsIndex = 6; // places end room at index 6
        }
        else
        {
            printf("Something, somewhere, went just a little bit wrong.");
        }

        for (j = 0; j < 7; j++) // iterate through array of room info: name, connections, type
        {
            if (j < k - 1)
            {
                strcpy(rooms[roomsIndex].connections[j - 1], splitArray[j]);
            }
            else
            {
                strcpy(rooms[roomsIndex].connections[j - 1], "NoConn"); // extra members of connections array
            }
        }
        strcpy(rooms[roomsIndex].name, splitArray[0]);
        strcpy(rooms[roomsIndex].type, splitArray[k - 1]);
    }
}

// prints current location to stdout
void PrintCurrentLocation(int currentLocation)
{
    printf("\nCURRENT LOCATION: %s\n", rooms[currentLocation].name);
}

// gets from file and prints possible connections
void GetAndPrintConnections(int currentLocation)
{
    int i = -6;
    printf("POSSIBLE CONNECTIONS: ");
    for (i = 0; i < 6; i++)
    {
        if ((strcmp(rooms[currentLocation].connections[i + 1], "NoConn") != 0) && (i < 5))
        {
            printf("%s, ", rooms[currentLocation].connections[i]); // name of connection followed by comma
        }
        else if (strcmp(rooms[currentLocation].connections[i], "NoConn") != 0)
        {
            printf("%s.\n", rooms[currentLocation].connections[i]); // name of final connection followed by period and new line
            break;
        }
    }
}

// takes pointer to a char, prints "WHERE TO? >" and gives user input to pointer
void SolicitUser(char* userInput)
{
	int numCharsEntered = -5;
	size_t bufferSize = 0;
	char* lineEntered = NULL;

    printf("WHERE TO? >");
    numCharsEntered = getline(&lineEntered, &bufferSize, stdin); // waits for input

    if (strcmp(lineEntered, "\n") == 0) // in case the user pushes "enter" only
    {
        strcpy(userInput, "bad user");
    }
    else
    {
        strcpy(userInput, strtok(lineEntered, "\n")); // not including newline
    }

    free(lineEntered);
}

// returns true if user input is the name of a room that is connected to the current room
bool IsValidConnection(int currentLocation, char* userInput)
{
    bool isValidConnection = false;
    int i = -6;
    for (i = 0; i < 6; i++) // interate through connection names
    {
        if (strcmp(rooms[currentLocation].connections[i], "NoConn") == 0) // no more connections
        {
            break;
        }
        if (strcmp(rooms[currentLocation].connections[i], userInput) == 0) // user input matches
        {
            isValidConnection = true;
        }
    }
    return isValidConnection;
}

// returns true if user input is the time command
bool IsTimeCommand(char* userInput)
{
    bool isTimeCommand = false;
    if (strcmp(userInput, "time") == 0)
    {
        isTimeCommand = true;
    }
    return isTimeCommand;
}

// returns properly formatted string of current time: " 1:03pm, Tuesday, September 13, 2016"
    // learned about time and strftime() here: https://www.tutorialspoint.com/c_standard_library/c_function_strftime.htm
void GetFormattedTimeString(char* currTime)
{
    time_t rawtime;
    struct tm *localTime;

    time( &rawtime );

    localTime = localtime( &rawtime );

    strftime(currTime, 40, "%l:%M%P, %A, %B %d, %Y\n", localTime); // %l gives two digits or space and single digit
}

// Creates time file and writes current time to file
void* CreateAndWriteTimeFile(void* argument)
{
    pthread_mutex_lock( &gameMutex );

    char *filePath = "./currentTime.txt";
    int file_descriptor = open(filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (file_descriptor == -1) // if file is not created
    {
        printf("open() failed on \"%s\"\n", filePath);
    }

    char *currTime = (char *) malloc(sizeof(char) * 42);
    memset(currTime, '\0', sizeof(char) * 42);
    GetFormattedTimeString(currTime); // modifies currTime

    size_t nwritten = write(file_descriptor, currTime, strlen(currTime) * sizeof(char));

    close(file_descriptor);
    free(currTime);
    pthread_mutex_unlock( &gameMutex );
}

// takes pointer to char array, reads time file and gives time string to pointer
void ReadTimeFile(char* currTime)
{
    char *filePath = "./currentTime.txt";
    FILE *file_pointer = fopen(filePath, "r");

    size_t numRead = fread(currTime, 40, 1, file_pointer); // read() stymied by commas

    fclose(file_pointer);
}

// takes validated user input and returns index of next current location
int FindNextCurrentLocation(char* userInput)
{
    int currentLocation = -1;
    int i = -7;
    for (i = 0; i < 7; i++) // iterates through names to find match
    {
        if (strcmp(rooms[i].name, userInput) == 0)
        {
            currentLocation = i; // we want index of matching name
            break;
        }
    }
    return currentLocation;
}

// prints "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN." if user input is not valid
void RoomNotValid()
{
    printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n");
}

// returns true if room is end room
bool IsEndRoom(int currentLocation)
{
    bool isEndRoom = false;
    if (currentLocation == 6)
    {
        isEndRoom = true;
    }
    return isEndRoom;
}