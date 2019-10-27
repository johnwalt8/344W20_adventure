/* test.c */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <math.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>

void fillRoomNameArray(char namesArray[10][10])
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

void chooseRoomNameAndType(int roomsArray[7])
{
    typedef enum { false, true } bool;
    int i = -7;
    int j = -7;
    // srand(time(NULL));

    for (i = 0; i < 7; i++)
    {
        int r = -7;
        while(roomsArray[i] == 7)
        {
            bool already = false;
            r = rand();
            r = r%10;
            for ( j = 0; j < i; j++)
            {
                if (r == roomsArray[j])
                {
                    already = true;
                }
            }
            if (already == false)
            {
                roomsArray[i] = r;
            }
        }
    }

}

int IsGraphFull(int connectionsArray[7][7])
{
    typedef enum { false, true } bool;
    bool atLeastThreeEach = true;
    int i = -7;
    int j = -7;
    int numConnectionsArray[7] = { 0, 0, 0, 0, 0, 0, 0 };
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < 7; j++)
        {
            if (connectionsArray[i][j] == true)
            {
                numConnectionsArray[i] = numConnectionsArray[i] + 1;
            }
        }
        if (numConnectionsArray[i] < 3)
        {
            atLeastThreeEach = false;
        }
    }
    return atLeastThreeEach;
}

int GetRandomRoom()
{
    int randomRoom = rand()%7;
    return randomRoom;
}

int CanAddConnectionFrom(int room, int connectionsArray[7][7])
{
    typedef enum { false, true } bool;
    bool canAddConnection = false;
    int i = -7;
    int connections = 0;
    for (i = 0; i < 7; i++)
    {
        if (connectionsArray[room][i] == true)
        {
            connections = connections + 1;
        }
    }
    if (connections < 6)
    {
        canAddConnection = true;
    }
    return canAddConnection;
}

int ConnectionAlreadyExists(int roomX, int roomY, int connectionsArray[7][7])
{
    return connectionsArray[roomX][roomY];
}

void ConnectRoom(int roomX, int roomY, int connectionsArray[7][7])
{
    typedef enum { false, true } bool;
    connectionsArray[roomX][roomY] = true;
}

int IsSameRoom(int roomX, int roomY)
{
    typedef enum { false, true } bool;
    bool isSameRoom = (roomX == roomY);
    return isSameRoom;
}

void AddRandomConnection(int connectionsArray[7][7])
{
    typedef enum { false, true } bool;
    int roomA = -7;
    int roomB = -7;

    while(true)
    {
        roomA = GetRandomRoom();

        if (CanAddConnectionFrom(roomA, connectionsArray) == true)
        {
            break;
        }
    }

    do
    {
        roomB = GetRandomRoom();
    }
    while(CanAddConnectionFrom(roomB, connectionsArray) == false || IsSameRoom(roomA, roomB) == true || ConnectionAlreadyExists(roomA, roomB, connectionsArray) == true);

    ConnectRoom(roomA, roomB, connectionsArray);
    ConnectRoom(roomB, roomA, connectionsArray);
}

int main()
{
    srand(time(NULL));

    typedef enum { false, true } bool;
    int i = -7;
    int j = -7;
    char roomNameArray[10][10];
    memset(roomNameArray, '\0', sizeof(roomNameArray));

    fillRoomNameArray(roomNameArray);

    int roomArray[7] = { 7, 7, 7, 7, 7, 7, 7 }; //room names randomly assigned to 0 through 6, 0 will be start, 6 will be end

    chooseRoomNameAndType(roomArray);

    int connectionsArray[7][7];
    for (i = 0; i < 7; i++)
    {
        for (j = 0; j < 7; j++)
        {
            connectionsArray[i][j] = false;
        }
    }

    while (IsGraphFull(connectionsArray) == false)
    {
        AddRandomConnection(connectionsArray);
    }

    // for (i = 0; i < 7; i++)
    // {
    //     for (j = 0; j < 7; j++)
    //     {
    //         printf("connectionsArray[%d][%d] = %d\n", i, j, connectionsArray[i][j]);
    //     }
    // }

    char dirName[25];
    char pidStr[10];
    memset(dirName, '\0', sizeof(dirName));
    memset(pidStr, '\0', sizeof(pidStr));

    strcpy(dirName, "johnwalt.rooms.");

    int pid = getpid();
    sprintf(pidStr, "%d", pid);

    strcat(dirName, pidStr);

    int result = mkdir(dirName, 0755);

    printf("dirName: %s, pid: %d, result: %d\n", dirName, pid, result);

    int file_descriptors[7];
    size_t nwritten;

    for (i = 0; i < 7; i++)
    {
        char filePath[36];
        char lineOfText[24];
        memset(filePath, '\0', sizeof(filePath));
        memset(lineOfText, '\0', sizeof(lineOfText));
        strcpy(filePath, "./");
        strcat(filePath, dirName);
        strcat(filePath, "/");
        strcat(filePath, roomNameArray[roomArray[i]]);
        strcat(filePath, "_room");
        printf("filePath: %s\n", filePath);

        file_descriptors[i] = open(filePath, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);

        if (file_descriptors[i] == -1)
        {
            printf("open() failed on \"%s\"\n", filePath);
        }

        strcpy(lineOfText, "ROOM_NAME: ");
        strcat(lineOfText, roomNameArray[roomArray[i]]);
        strcat(lineOfText, "\n");

        // printf("lineOfText: %s\n", lineOfText);

        nwritten = write(file_descriptors[i], lineOfText, strlen(lineOfText) * sizeof(char));

        // printf("nwritten: %ld\n", nwritten);

        int connection = 1;
        for (j = 0; j < 7; j++)
        {
            if (connectionsArray[i][j])
            {
                memset(lineOfText, '\0', sizeof(lineOfText));
                sprintf(lineOfText, "CONNECTION %d: ", connection);
                connection++;
                strcat(lineOfText, roomNameArray[roomArray[j]]);
                strcat(lineOfText, "\n");
                nwritten = write(file_descriptors[i], lineOfText, strlen(lineOfText) * sizeof(char));
            }
        }

        memset(lineOfText, '\0', sizeof(lineOfText));
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
        nwritten = write(file_descriptors[i], lineOfText, strlen(lineOfText) * sizeof(char));
    }

    return 0;
}