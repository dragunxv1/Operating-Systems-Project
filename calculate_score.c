#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "manager.h"

typedef struct {
  char name[MAX];
  int score;
} USER;

USER userList[MAX];
int userCounter = 0;

void printUserList(char *hunt) {
  printf("Scores for hunt %s are:\n", hunt);
  for (int i = 0; i < userCounter; i++) {
    printf("%s : %d\n", userList[i].name, userList[i].score);
  }
}

int userExists(char *name) {
  for (int i = 0; i < userCounter; i++) {
    if (strcmp(userList[i].name, name) == 0) {
      return i;
    }
  }
  return 0;
}

int calculateScore(char *hunt) {
  char *dataPath = dataFilepath(hunt);
  if (dataPath == NULL) {
    printf("Data Path not found.");
    return -1;
  }
  int dataFile = open(dataPath, O_RDONLY);
  if (dataFile == -1) {
    perror("Hunt not found :");
    close(dataFile);
    free(dataPath);
    return -1;
  }

  free(dataPath);
  Treasure tr = {0};
  int scoreIndex = 0;

  while (1) {
    if (read(dataFile, &tr, sizeof(tr)) != sizeof(tr)) {
      break;
    }

    scoreIndex = userExists(tr.userName);
    if (!scoreIndex) {
      strcpy(userList[userCounter].name, tr.userName);
      userList[userCounter++].score = tr.value;
    } else {
      userList[scoreIndex].score += tr.value;
    }
  }

  if (close(dataFile) == -1) {
    perror("File close error :");
    return -1;
  }

  return 0;
}

int main(int argc, char **argv) {
  calculateScore(argv[1]);
  printUserList(argv[1]);
  return 0;
}