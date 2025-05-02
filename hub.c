#include "hub.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "manager.h"

pid_t monitorID = -1;
int monitorStatus = 0;

char *askForInput() {
  char buff[50];
  fgets(buff, 50, stdin);

  char *input = (char *)malloc(strlen(buff) + 1);
  if (input == NULL) {
    perror("Input Malloc Error :");
    return NULL;
  }
  strcpy(input, buff);
  return input;
}

void displayMenu() {
  printf("Choose a command:\n");
  printf("1) Start Monitor.\n");
  printf("2) List Hunts.\n");
  printf("3) List Treasures.\n");
  printf("4) View Treasure.\n");
  printf("5) Stop Monitor.\n");
  printf("0) EXIT.\n");
}

int getNumberOfHunts(char *hunt) {
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
  int bytesRead = 0;
  bytesRead = lseek(dataFile, 0, SEEK_END);
  if (bytesRead == -1) {
    perror("LSEEK Error :");
    return -1;
  }

  return bytesRead / sizeof(Treasure);
}

void huntLIST() {
  struct dirent *entry;
  DIR *directory = opendir(".");

  if (directory == NULL) {
    perror("opendir");
    return;
  }

  while ((entry = readdir(directory))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    char fullPath[1024];
    snprintf(fullPath, sizeof(fullPath), "%s/%s", ".", entry->d_name);

    struct stat statbuf;
    if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      if (strcmp(entry->d_name, ".git") == 0) {
        continue;
      }
      printf("Hunt Name: %s ", entry->d_name);
      printf("Number of Treasures: %d\n", getNumberOfHunts(entry->d_name));
    }
  }

  closedir(directory);
}

void signalHandler(int signal) {
  if (signal == SIGUSR1) {
    system("clear");
    kill(getppid(), SIGSTOP);
    printf("LIST HUNTS\n");
    huntLIST(".");
    kill(getppid(), SIGCONT);
    displayMenu();

  } else if (signal == SIGUSR2) {
    system("clear");
    kill(getppid(), SIGSTOP);
    printf("LIST TREASURES\n");

    char command[100];
    printf("Type HUNT name:");
    char *hunt = askForInput();
    sprintf(command, "./treasure_manager --list %s", hunt);
    system(command);

    kill(getppid(), SIGCONT);
    displayMenu();
    free(hunt);

  } else if (signal == SIGINT) {
    system("clear");
    kill(getppid(), SIGSTOP);
    printf("VIEW TREASURES\n");

    char command[100];
    printf("Type HUNT name:");
    char *hunt = askForInput();
    hunt[strcspn(hunt, "\n")] = '\0';
    printf("Type Treasure ID:");
    int tresID = 0;
    if (scanf("%d", &tresID) != 1) {
      perror("Treasure ID SCANF Error");
    }
    getchar();

    sprintf(command, "./treasure_manager --view %s %d", hunt, tresID);
    printf("%s\n", command);
    system(command);

    kill(getppid(), SIGCONT);
    displayMenu();
    free(hunt);

  } else if (signal == SIGTERM) {
    system("clear");
    printf("TERMINATING MONITOR\n");
    _exit(0);
  }
}

int startMonitor() {
  if (monitorStatus == 1) {
    system("clear");
    printf("Monitor is already running.\n");
    return -1;
  }

  monitorID = fork();
  if (monitorID < 0) {
    perror("Monitor did not start :");
    return -1;
  }
  if (monitorID == 0) {
    struct sigaction sa = {0};
    sa.sa_handler = signalHandler;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    while (1) {
      pause();
    }

  } else {
    monitorStatus = 1;
    system("clear");
    printf("Monitor Started Successfully.\n");
  }
  return 0;
}

int listHunts() {
  if (monitorStatus == 0) {
    system("clear");
    printf("Monitor not running.\n");
    return -1;
  }
  if (kill(monitorID, SIGUSR1) == -1) {
    perror("Signal not sent :");
    return -1;
  }
  return 0;
}

int listTreasures2() {
  if (monitorStatus == 0) {
    system("clear");
    printf("Monitor not running.\n");
    return -1;
  }
  if (kill(monitorID, SIGUSR2) == -1) {
    perror("Signal not sent :");
    return -1;
  }
  return 0;
}

int viewTreasure2() {
  if (monitorStatus == 0) {
    system("clear");
    printf("Monitor not running.\n");
    return -1;
  }
  if (kill(monitorID, SIGINT) == -1) {
    perror("Signal not sent :");
    return -1;
  }
  return 0;
}

int stopMonitor() {
  if (monitorStatus == 0) {
    system("clear");
    printf("Monitor not running.\n");
    return -1;
  }
  if (kill(monitorID, SIGTERM) == -1) {
    perror("Signal not sent :");
    return -1;
  }
  waitpid(monitorID, NULL, 0);
  monitorStatus = 0;
  return 0;
}

int closeProgram() {
  if (monitorStatus != 0) {
    system("clear");
    printf("Program cannot exit while monitor is still running.\n");
    return -1;
  }
  return 0;
}
