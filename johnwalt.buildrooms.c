/* johnwalt.buildrooms.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

typedef enum { false, true } bool; // from: https://stackoverflow.com/questions/1921539/using-boolean-values-in-c
                                    // because I love booleans

// Hardcode ten names, seven of which will be randomly assigned to rooms
void FillRoomNameArray(char namesArray[10][10])
{
    strcpy(namesArray[0], "Asimov");
    strcpy(namesArray[1], "Heinline");
    strcpy(namesArray[2], "Harrison");
    strcpy(namesArray[3], "Brin");
    strcpy(namesArray[4], "LeGuin");
    strcpy(namesArray[5], "Morgan");
    strcpy(namesArray[6], "Delany");
    strcpy(namesArray[7], "McAuley");
    strcpy(namesArray[8], "Corey");
    strcpy(namesArray[9], "Hamilton");
}

// takes array of seven ints representing rooms 1 through 7, assigns name to each
    // room 0 will be start, room 6 will be end, names randomly assigned to these positions
void ChooseRoomNameAndType(int roomsArray[7])
{
    int i = -7;
    int j = -7;

    for (i = 0; i < 7; i++) // iterate through rooms 0 through 6
    {
        int r = -7;
        while(roomsArray[i] == 7)
        {
            bool already = false; // name not already used
            r = rand();
            r = r%10; // random integer 0 through 9
            for ( j = 0; j < i; j++) // check all names
            {
                if (r == roomsArray[j])
                {
                    already = true; // name already used
                }
            }
            if (already == false) // name not used, can assign it to current room
            {
                roomsArray[i] = r;
            }
        }
    }

}

// Returns true if all rooms have 3 to 6 outbound connections, false otherwise
int IsGraphFull(int connectionsArray[7][7])
{
    bool atLeastThreeEach = true;
    int i = -7;
    int j = -7;
    int numConnectionsArray[7] = { 0, 0, 0, 0, 0, 0, 0 }; // each room and its number of connections
    for (i = 0; i < 7; i++) // check all seven rooms
    {
        for (j = 0; j < 7; j++) // check each possible connection
        {
            if (connectionsArray[i][j] == true)
            {
                numConnectionsArray[i] = numConnectionsArray[i] + 1;
            }
        }
        if (numConnectionsArray[i] < 3) // if any room has less than 3 connections
        {
            atLeastThreeEach = false;
        }
    }
    return atLeastThreeEach;
}

// Returns a random Room, does NOT validate if connection can be added
int GetRandomRoom()
{
    int randomRoom = rand()%7; // simply generates a random integer from 0 to 6
    return randomRoom;
}

// Returns true if a connection can be added from Room x (< 6 outbound connections), false otherwise
int CanAddConnectionFrom(int room, int connectionsArray[7][7])
{
    bool canAddConnection = false;
    int i = -7;
    int connections = 0;
    for (i = 0; i < 7; i++) // checks every possible connection
    {
        if (connectionsArray[room][i] == true)
        {
            connections = connections + 1; // keeps track of number of connections
        }
    }
    if (connections < 6) // if less than 6 connections, then there's room for another
    {
        canAddConnection = true;
    }
    return canAddConnection;
}

// Returns true if a connection from Room x to Room y already exists, false otherwise
int ConnectionAlreadyExists(int roomX, int roomY, int connectionsArray[7][7])
{
    return connectionsArray[roomX][roomY];
}

// Connects Rooms x and y together, does not check if this connection is valid
void ConnectRoom(int roomX, int roomY, int connectionsArray[7][7])
{
    connectionsArray[roomX][roomY] = true;
}

// Returns true if Rooms x and y are the same Room, false otherwise
int IsSameRoom(int roomX, int roomY)
{
    bool isSameRoom = (roomX == roomY);
    return isSameRoom;
}

// Adds a random, valid outbound connection from a Room to another Room
void AddRandomConnection(int connectionsArray[7][7])
{
    int roomA = -7;
    int roomB = -7;

    while(true) // get a room that has room for another connection
    {
        roomA = GetRandomRoom();

        if (CanAddConnectionFrom(roomA, connectionsArray) == true)
        {
            break;
        }
    }

    do // get a room to connect to, must have room for a connection, can't be the same room, can't already be connected
    {
        roomB = GetRandomRoom();
    }
    while(CanAddConnectionFrom(roomB, connectionsArray) == false || IsSameRoom(roomA, roomB) == true || ConnectionAlreadyExists(roomA, roomB, connectionsArray) == true);

    ConnectRoom(roomA, roomB, connectionsArray);
    ConnectRoom(roomB, roomA, connectionsArray); // make the reverse connection while we have needed info
}

// uses array of rooms, names, and connections to create room files
void CreateAndPrintRoomFiles(char roomNameArray[10][10], int roomArray[7], int connectionsArray[7][7])
{
    char dirName[25]; // name of directory, will include process id number suffix
    char pidStr[10]; // process id number as a string
    memset(dirName, '\0', sizeof(dirName));
    memset(pidStr, '\0', sizeof(pidStr));

    strcpy(dirName, "johnwalt.rooms.");
    int pid = getpid();  // process id number as an integer
    sprintf(pidStr, "%d", pid); // cast integer as string and assign to string variable
    strcat(dirName, pidStr); // add process id number to directory name

    int result = mkdir(dirName, 0755); // create directory with permissions

    int file_descriptor;
    size_t nwritten;
    int i = -7;
    int j = -7;

    for (i = 0; i < 7; i++) // iterate through all seven rooms
    {
        char filePath[36]; // name of room file inside created directory
        char lineOfText[24]; // for each line inside each room file
        memset(filePath, '\0', sizeof(filePath));
        memset(lineOfText, '\0', sizeof(lineOfText));

        strcpy(filePath, "./");
        strcat(filePath, dirName);
        strcat(filePath, "/");
        strcat(filePath, roomNameArray[roomArray[i]]); // get the room name
        strcat(filePath, "_room");

        file_descriptor = open(filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        if (file_descriptor == -1) // if file is not created
        {
            printf("open() failed on \"%s\"\n", filePath);
        }

        strcpy(lineOfText, "ROOM_NAME: "); // first line in room file
        strcat(lineOfText, roomNameArray[roomArray[i]]);
        strcat(lineOfText, "\n");

        nwritten = write(file_descriptor, lineOfText, strlen(lineOfText) * sizeof(char));

        int connection = 1;
        for (j = 0; j < 7; j++) // write a line for each connection
        {
            if (connectionsArray[i][j])
            {
                memset(lineOfText, '\0', sizeof(lineOfText)); // empty string variable
                sprintf(lineOfText, "CONNECTION %d: ", connection);
                connection++;
                strcat(lineOfText, roomNameArray[roomArray[j]]);
                strcat(lineOfText, "\n");
                nwritten = write(file_descriptor, lineOfText, strlen(lineOfText) * sizeof(char));
            }
        }

        memset(lineOfText, '\0', sizeof(lineOfText)); // empty string variable
        strcpy(lineOfText, "ROOM_TYPE: ");
        switch (i)
        {
            case 0:
                strcat(lineOfText, "START_ROOM\n");
                break;
            case 1: case 2: case 3: case 4: case 5:
                strcat(lineOfText, "MID_ROOM\n");
                break;
            case 6:
                strcat(lineOfText, "END_ROOM\n");
                break;
            default:
                printf("Something, somewhere, went just a little bit wrong.");
                break;
        }
        nwritten = write(file_descriptor, lineOfText, strlen(lineOfText) * sizeof(char));
    }
}

int main()
{
    srand(time(NULL)); // initialize seed for rand()

    char roomNameArray[10][10]; // Array of 10 name strings, 7 of which will be randomly chosen
    memset(roomNameArray, '\0', sizeof(roomNameArray));
    FillRoomNameArray(roomNameArray);

    int roomArray[7] = { 7, 7, 7, 7, 7, 7, 7 }; // Room names will be randomly assigned to 0 through 6
                                                // 0 will be start, 6 will be end
    ChooseRoomNameAndType(roomArray);

    int i = -7;
    int j = -7;
    int connectionsArray[7][7]; // Array of connections between rooms, each room (0 - 6) will have 7 possible connections (one of which, connection to self, will never be used), assigned randomly
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < 7; j++)
        {
            connectionsArray[i][j] = false; // initially all connections false
        }
    }

    while (IsGraphFull(connectionsArray) == false)
    {
        AddRandomConnection(connectionsArray);
    }

    CreateAndPrintRoomFiles(roomNameArray, roomArray, connectionsArray);

    return 0;
}