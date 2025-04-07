#define treasureTXT 50
#define MAX 100

typedef struct GPS{
    float x;
    float y;
}GPS;

typedef struct Treasure{
    char userName[treasureTXT];
    char clue[treasureTXT];
    GPS gps;
    int ID;
    int value;
}Treasure;

void treasurePrint(Treasure x);
int determineOperation(char *operation);
char* dataFilepath(char *hunt);
char* logFilepath(char *hunt);
Treasure treasureRead(Treasure x);