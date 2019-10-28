/* johnwalt.adventure.c */

#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
// #include <sys/types.h>
// #include <errno.h>

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
void PrintCurrentLocation(char currentLocation[10])
{

}

// gets from file and prints possible connections
void GetAndPrintConnections()
{

}

// prints "WHERE TO? >" and waits for user input
void SolicitUser()
{

}

// returns true if user input is the name of a room
int IsValidRoom()
{
    
}

// prints "HUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN." if user input is not valid
void RoomNotValid()
{

}

// returns true if room is end room
int IsEndRoom()
{

}

int main()
{
    int i = -7;
    int j = -6;
    char *dirName = GetRoomsDirectory();
    printf("dirName: %s\n", dirName);

    char **roomsArray = GetRoomNames(dirName);
    for (i = 0; i < 7; i++)
    {
        printf("Room %d: %s\n", i, roomsArray[i]);
    }

    FillArrayOfRoomStructs(dirName, roomsArray);

    for (i = 0; i < 7; i++)
    {
        printf("rooms[%d].name: %s\n", i, rooms[i].name);
        printf("rooms[%d].type: %s\n", i, rooms[i].type);
        for (j = 0; j < 6; j++)
        {
            if (strcmp(rooms[i].connections[j], "NoConn") != 0)
            {
                printf("rooms[%d].connections[%d]: %s\n", i, j, rooms[i].connections[j]);
            }
        }
        printf("\n");
    }


    return 0;
}