/* johnwalt.adventure.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <time.h>

typedef enum { false, true } bool; // from: https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

struct room
{
    char name[12];
    char type[12];
    char connections[6][12];
};

struct room rooms[7];

// returns name of most recent rooms directory
    // from reading 2.4 Manipulating Directories
char * GetRoomsDirectory()
{
    int newestDirTime = -1;
    char targetDirPrefix[32] = "johnwalt.rooms.";
    char newestDirName[32];
    char *dirName = (char *) malloc(sizeof(char) * 32);
    memset(newestDirName, '\0', sizeof(newestDirName));

    DIR* dirToCheck;
    struct dirent *fileInDir; 
    struct stat dirAttributes;

    dirToCheck = opendir(".");

    if (dirToCheck > 0)
    {
        while ((fileInDir = readdir(dirToCheck)) != NULL)
        {
            if (strstr(fileInDir->d_name, targetDirPrefix) != NULL)
            {
                stat(fileInDir->d_name, &dirAttributes);

                if ((int)dirAttributes.st_mtime > newestDirTime)
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
    strcat(dirName, newestDirName);

    return dirName;
}

// returns an array of names of seven rooms
char** GetRoomNames(char *dirName)
{
    int i = -7;
    char **roomsArray = (char**) malloc(sizeof(char*) * 7);
    for (i = 0; i < 7; i++)
    {
        roomsArray[i] = (char*) malloc(sizeof(char) * 16);
    }
    for (i = 0; i < 7; i++)
    {
        memset(roomsArray[i], '\0', sizeof(char) * 16);
    }

    i = 0;
    DIR* roomDir;
    struct dirent *fileInDir;
    roomDir = opendir(dirName);
    if (roomDir > 0)
    {
        while ((fileInDir = readdir(roomDir)) != NULL)
        {
            if (fileInDir->d_type == DT_REG && i < 7)
            {
                strcpy(roomsArray[i], strtok(fileInDir->d_name, "_"));
                i++;
            }
        }
    }
    
    closedir(roomDir);

    return roomsArray;
}

// gets room info from room files and fills array of room structs called rooms
void FillArrayOfRoomStructs(char dirName[32], char** roomsArray)
{
    int i = -7;
    int j = -8;
    int file_descriptor;
    char filePath[48];
    char readBuffer[256];
    size_t nread;
    const char newline[3] = "\n";
    const char colonSpace[3] = ":";
    char *tempString;
    char tempArray[8][24];
    char splitArray[8][10];
    int midRoom = 0;

    for (i = 0; i < 7; i++)
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
        strcat(filePath, "_room");

        file_descriptor = open(filePath, O_RDONLY);

        if (file_descriptor == -1)
        {
            printf("open() failed on \"%s\"\n", filePath);
        }

        memset(readBuffer, '\0', sizeof(readBuffer));
        nread = read(file_descriptor, readBuffer, sizeof(readBuffer));

        tempString = strtok(readBuffer, newline);
        int k = 0;
        strcpy(tempArray[k], tempString);

        while (tempString != NULL)
        {
            strcpy(tempArray[k], tempString);
            tempString = strtok(NULL, newline);
            k++;
        }

        for (j = 0; j < k; j++)
        {
            strtok(tempArray[j], colonSpace);
            tempString = strtok(NULL, colonSpace);
            sscanf(tempString, "%s", splitArray[j]);
        }

        int roomsIndex = 0;
        if (strcmp(splitArray[k - 1], "START_ROOM") == 0)
        {
            roomsIndex = 0;
        }
        else if (strcmp(splitArray[k - 1], "MID_ROOM") == 0)
        {
            midRoom++;
            roomsIndex = midRoom;
        }
        else if (strcmp(splitArray[k - 1], "END_ROOM") == 0)
        {
            roomsIndex = 6;
        }
        else
        {
            printf("Something, somewhere, went just a little bit wrong.");
        }

        for (j = 0; j < 7; j++)
        {
            if (j < k - 1)
            {
                strcpy(rooms[roomsIndex].connections[j - 1], splitArray[j]);
            }
            else
            {
                strcpy(rooms[roomsIndex].connections[j - 1], "NoConn");
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
            printf("%s, ", rooms[currentLocation].connections[i]);
        }
        else if (strcmp(rooms[currentLocation].connections[i], "NoConn") != 0)
        {
            printf("%s.\n", rooms[currentLocation].connections[i]);
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
    numCharsEntered = getline(&lineEntered, &bufferSize, stdin);

    if (strcmp(lineEntered, "\n") == 0)
    {
        strcpy(userInput, "bad user");
    }
    else
    {
        strcpy(userInput, strtok(lineEntered, "\n"));
    }

    free(lineEntered);
}

// returns true if user input is the name of a room that is connected to the current room
bool IsValidConnection(int currentLocation, char* userInput)
{
    bool isValidConnection = false;
    int i = -6;
    for (i = 0; i < 6; i++)
    {
        if (strcmp(rooms[currentLocation].connections[i], "NoConn") == 0)
        {
            break;
        }
        if (strcmp(rooms[currentLocation].connections[i], userInput) == 0)
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
    struct tm *info;

    time( &rawtime );

    info = localtime( &rawtime );

    strftime(currTime, 40, "%l:%M%P, %A, %B %d, %Y\n", info);
}

// Creates time file and writes current time to file
void CreateAndWriteTimeFile()
{
    char *filePath = "./currentTime.txt";
    int file_descriptor = open(filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (file_descriptor == -1) // if file is not created
    {
        printf("open() failed on \"%s\"\n", filePath);
    }

    char *currTime = (char *) malloc(sizeof(char) * 40);
    GetFormattedTimeString(currTime);

    size_t nwritten = write(file_descriptor, currTime, strlen(currTime) * sizeof(char));

    close(file_descriptor);
    free(currTime);
}

// takes pointer to char array, reads time file and gives time string to pointer
void ReadTimeFile(char* currTime)
{
    char readBuffer[42];
    memset(readBuffer, '\0', sizeof(readBuffer));

    char *filePath = "./currentTime.txt";
    FILE *file_pointer = fopen(filePath, "r");

    size_t numRead = fread(currTime, 40, 1, file_pointer);
    
    fclose(file_pointer);
}

// takes validated user input and returns index of current location
int FindNextCurrentLocation(char* userInput)
{
    int currentLocation = -1;
    int i = -7;
    for (i = 0; i < 7; i++)
    {
        if (strcmp(rooms[i].name, userInput) == 0)
        {
            currentLocation = i;
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

int main()
{
    int i = -7;
    int j = -6;

    char *dirName = GetRoomsDirectory();

    char **roomsArray = GetRoomNames(dirName);

    FillArrayOfRoomStructs(dirName, roomsArray);

    int currentLocation = 0;
    char *userInput = (char *) malloc(sizeof(char) * 128);
    bool isEndRoom = false;
    int steps = 0;
    int roomIndexes[64];

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
            if (isValidConnection == true)
            {
                currentLocation = FindNextCurrentLocation(userInput);
                roomIndexes[steps] = currentLocation;
                steps++;
            }
            else if (IsTimeCommand(userInput) == true)
            {
                CreateAndWriteTimeFile();
                char *currTime = (char *) malloc(sizeof(char) * 40);
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
    free(roomsArray);
    free(userInput);

    return 0;
}