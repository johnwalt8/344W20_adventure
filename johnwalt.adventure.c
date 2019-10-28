/* johnwalt.adventure.c */

#include <stdio.h>
#include <stdlib.h>
// #include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
// #include <fcntl.h>
#include <dirent.h>
// #include <sys/types.h>

typedef enum { false, true } bool; // from: https://stackoverflow.com/questions/1921539/using-boolean-values-in-c

// gets most recent rooms directory
    // from reading 2.4 Manipulating Directories
char * GetRoomsDirectory()
{
    int newestDirTime = -1;
    char targetDirPrefix[32] = "johnwalt.rooms.";
    char *newestDirName = (char *) malloc(sizeof(char) * 256);
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

    return newestDirName;
}

// gets name of start room from files
void GetStartRoom(char startRoom[10])
{

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
    char *dirName = GetRoomsDirectory();

    printf("dirName: %s\n", dirName);

    return 0;
}