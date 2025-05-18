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

int status = 0;
pid_t monitorID = -1;
int monitorStatus = 0;

int pipefd[2];
char buffer[MAX * MAX];
char cleanBuffer[MAX * MAX] = {0};
int savedSTDOUT = 0;

char *askForInput() {
  char buff[MAX / 2];
  fgets(buff, MAX / 2, stdin);

  char *input = (char *)malloc(strlen(buff) + 1);
  if (input == NULL) {
    perror("Input Malloc Error :");
    return NULL;
  }
  strcpy(input, buff);
  return input;
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
    perror("OPENDIR ERROR :");
    return;
  }

  char fullPath[3 * MAX];
  struct stat statbuf;

  while ((entry = readdir(directory))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    snprintf(fullPath, sizeof(fullPath), "%s/%s", ".", entry->d_name);

    if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      if (strcmp(entry->d_name, ".git") == 0) {
        continue;
      }
      printf("Hunt Name: %s ", entry->d_name);
      printf("Number of Treasures: %d\n", getNumberOfHunts(entry->d_name));
    }
  }
  printf("\n");
  closedir(directory);
}

void mainSignalHandler(int signal) {
  if (signal == SIGUSR1) {
    memset(buffer, 0, sizeof(buffer));
    if (read(pipefd[0], buffer, sizeof(buffer)) == -1) {
      printf("mainSignalHandler read error.");
      return;
    }
    printf("%s\n", buffer);
  }
}

void monitorSignalHandler(int signal) {
  kill(getppid(), SIGCONT);
  if (signal == SIGUSR1) {
    system("clear");
    printf("LIST HUNTS\n");

    savedSTDOUT = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    huntLIST(".");
    dup2(savedSTDOUT, STDOUT_FILENO);

    kill(getppid(), SIGUSR1);

    printf("\n");

  } else if (signal == SIGUSR2) {
    system("clear");
    printf("LIST TREASURES\n");

    char command[MAX];
    printf("Type HUNT name:");

    char *hunt = askForInput();
    hunt[strcspn(hunt, "\n")] = '\0';

    printf("\n");
    sprintf(command, "./treasure_manager --list %s", hunt);
    free(hunt);

    savedSTDOUT = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    system(command);
    dup2(savedSTDOUT, STDOUT_FILENO);

    kill(getppid(), SIGUSR1);

    printf("\n");

  } else if (signal == SIGINT) {
    system("clear");
    printf("VIEW TREASURES\n");

    char command[MAX];
    printf("Type HUNT name:");
    char *hunt = askForInput();
    hunt[strcspn(hunt, "\n")] = '\0';
    printf("Type Treasure ID:");
    int tresID = 0;
    if (scanf("%d", &tresID) != 1) {
      perror("Treasure ID SCANF Error");
    }
    getchar();

    printf("\n");
    sprintf(command, "./treasure_manager --view %s %d", hunt, tresID);
    free(hunt);

    savedSTDOUT = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    system(command);
    dup2(savedSTDOUT, STDOUT_FILENO);

    kill(getppid(), SIGUSR1);

    printf("\n");

  } else if (signal == SIGTERM) {
    system("clear");
    printf("TERMINATING MONITOR\n");
    kill(getppid(), SIGCONT);
    close(pipefd[1]);
    _exit(0);
  }
}

int startMonitor() {
  if (monitorStatus == 1) {
    system("clear");
    printf("Monitor is already running.\n");
    return -1;
  }

  struct sigaction sa = {0};
  sa.sa_handler = mainSignalHandler;

  sigaction(SIGUSR1, &sa, NULL);

  if (pipe(pipefd) == -1) {
    perror("Pipe creation error :");
    return -1;
  }

  monitorID = fork();
  if (monitorID < 0) {
    perror("Monitor did not start :");
    return -1;
  }

  if (monitorID == 0) {
    struct sigaction sa = {0};
    sa.sa_handler = monitorSignalHandler;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    close(pipefd[0]);

    while (1) {
      pause();
    }

  } else {
    monitorStatus = 1;
    system("clear");
    printf("Monitor Started Successfully.\n");
    close(pipefd[1]);
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
  pause();
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
  pause();
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
  pause();
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
  close(pipefd[0]);
  monitorStatus = 0;
  waitpid(monitorID, &status, 0);
  printf("Monitor ended with status %d\n", WEXITSTATUS(status));
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

int calculateScore() {
  system("clear");

  struct dirent *entry;
  DIR *directory = opendir(".");

  if (directory == NULL) {
    perror("OPENDIR ERROR :");
    return -1;
  }

  char directoryList[MAX][MAX] = {0};
  int directoryCounter = 0;
  char fullPath[3 * MAX] = {0};
  char command[3 * MAX] = {0};
  struct stat statbuf;
  int processPipe[2] = {0};

  if (pipe(processPipe) == -1) {
    perror("Pipe Creation Failed :");
    return -1;
  }

  while ((entry = readdir(directory))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    snprintf(fullPath, sizeof(fullPath), "%s/%s", ".", entry->d_name);

    if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      if (strcmp(entry->d_name, ".git") == 0) {
        continue;
      }
      strcpy(directoryList[directoryCounter++], entry->d_name);
    }
  }

  closedir(directory);

  for (int i = 0; i < directoryCounter; i++) {
    pid_t pid = fork();
    if (pid == 0) {
      close(processPipe[0]);

      memset(command, 0, sizeof(command));
      sprintf(command, "./calculate_score %s", directoryList[i]);

      dup2(processPipe[1], STDOUT_FILENO);
      close(processPipe[1]);
      system(command);

      exit(0);
    }
  }

  for (int i = 0; i < directoryCounter; i++) {
    wait(NULL);
  }

  char buffer[MAX * MAX] = {0};
  read(processPipe[0], buffer, sizeof(buffer) - 1);
  printf("%s", buffer);

  printf("\n");
  return 0;
}
