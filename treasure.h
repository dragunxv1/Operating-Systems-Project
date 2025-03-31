#define treasureTXT 50

typedef struct GPS{
    float x;
    float y;
}GPS;

typedef struct Treasure{
    GPS gps;
    int ID;
    int value;
    char userName[treasureTXT];
    char clue[treasureTXT];
}Treasure;

void treasurePrint(Treasure x);