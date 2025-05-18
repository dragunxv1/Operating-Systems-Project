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
int savedSTDOUT = 0;

char *askForInput() {
  char buff[MAX / 2] = {0};
  if(fgets(buff, MAX / 2, stdin) == NULL){
    perror("askForInput :: fgets error.");
    return NULL;
  }

  char *input = (char *)malloc(strlen(buff) + 1);
  if (input == NULL) {
    perror("askForInput :: Input Malloc Error :");
    return NULL;
  }
  strcpy(input, buff);
  return input;
}

int getNumberOfHunts(char *hunt) {
  char *dataPath = dataFilepath(hunt);
  if (dataPath == NULL) {
    printf("getNumberOfHunts :: Data Path not found.");
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
  struct dirent *entry = {0};
  DIR *directory = opendir(".");

  if (directory == NULL) {
    perror("huntLIST :: OPENDIR ERROR :");
    return;
  }

  char fullPath[3 * MAX] = {0};
  struct stat statbuf = {0};

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
  if (signal == SIGUSR1) {
    system("clear");
    printf("LIST HUNTS\n");

    savedSTDOUT = dup(STDOUT_FILENO);
    if(savedSTDOUT == -1){
      printf("monitorSignalHandler SIGUSR1 : save STDOUT error.");
      return;
    }
    if(dup2(pipefd[1], STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGUSR1 : STDOUT to pipe redirect error.");
      return;
    }
    huntLIST(".");
    if(dup2(savedSTDOUT, STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGUSR1 : STDOUT restore error.");
      return;
    }

    if(kill(getppid(), SIGUSR1) == -1){
      printf("monitorSignalHandler SIGUSR1 : Failed to send signal with KILL.");
      return;
    }

    printf("\n");

  } else if (signal == SIGUSR2) {
    system("clear");
    printf("LIST TREASURES\n");

    char command[MAX] = {0};
    printf("Type HUNT name:");

    char *hunt = askForInput();
    if(hunt == NULL){
      printf("monitorSignalHandler SIGUSR2 : Failed to read input.");
      return;
    }
    hunt[strcspn(hunt, "\n")] = '\0';

    printf("\n");
    if(snprintf(command, sizeof(command), "./treasure_manager --list %s", hunt) < 0){
      printf("monitorSignalHandler SIGUSR2: Failed to create command.");
      free(hunt);
      return;
    }
    free(hunt);

    savedSTDOUT = dup(STDOUT_FILENO);
    if(savedSTDOUT == -1){
      printf("monitorSignalHandler SIGUSR2 : save STDOUT error.");
      return;
    }
    if(dup2(pipefd[1], STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGUSR2 : STDOUT to pipe redirect error.");
      return;
    }

    system(command);

    if(dup2(savedSTDOUT, STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGUSR2 : STDOUT restore error.");
      return;
    }

    
    if(kill(getppid(), SIGUSR1) == -1){
      printf("monitorSignalHandler SIGUSR2 : Failed to send signal with KILL.");
      return;
    }
    
    printf("\n");

  } else if (signal == SIGINT) {
    system("clear");
    printf("VIEW TREASURES\n");

    char command[MAX] = {0};
    printf("Type HUNT name:");
    char *hunt = askForInput();
    if(hunt == NULL){
      printf("monitorSignalHandler SIGINT : Failed to read input.");
      return;
    }
    hunt[strcspn(hunt, "\n")] = '\0';
    printf("Type Treasure ID:");
    int tresID = 0;
    if (scanf("%d", &tresID) != 1) {
      perror("monitorSignalHandler SIGINT : Treasure ID scanf error.");
      free(hunt);
    }
    getchar();

    printf("\n");
    if(snprintf(command,sizeof(command) ,"./treasure_manager --view %s %d", hunt, tresID) < 0){
      printf("monitorSignalHandler SIGINT : Failed to create command.");
      free(hunt);
      return;
    }
    free(hunt);

    savedSTDOUT = dup(STDOUT_FILENO);
    if(savedSTDOUT == -1){
      printf("monitorSignalHandler SIGINT : save STDOUT error.");
      return;
    }

    if(dup2(pipefd[1], STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGINT : STDOUT to pipe redirect error.");
      return;
    }

    system(command);

    if(dup2(savedSTDOUT, STDOUT_FILENO) == -1){
      printf("monitorSignalHandler SIGINT : STDOUT restore error.");
      return;
    }

    if(kill(getppid(), SIGUSR1) == -1){
      printf("monitorSignalHandler SIGINT : Failed to send signal with KILL.");
      return;
    }

    printf("\n");

  } else if (signal == SIGTERM) {
    system("clear");
    printf("TERMINATING MONITOR\n");
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

  if(sigaction(SIGUSR1, &sa, NULL) == -1){
    printf("startMonitor MAIN sigaction error.");
    return -1;
  }

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

    if(sigaction(SIGUSR1, &sa, NULL) == -1){
      printf("startMonitor MONITOR sigaction error.");
      return -1;
    }
    if(sigaction(SIGUSR2, &sa, NULL) == -1){
      printf("startMonitor MONITOR sigaction error.");
      return -1;
    }
    if(sigaction(SIGINT, &sa, NULL) == -1){
      printf("startMonitor MONITOR sigaction error.");
      return -1;
    }
    if(sigaction(SIGTERM, &sa, NULL) == -1){
      printf("startMonitor MONITOR sigaction error.");
      return -1;
    }

    if(close(pipefd[0]) == -1){
      printf("startMonitor MONITOR close pipe read error.");
      return -1;
    }

    while (1) {
      pause();
    }

  } else {
    monitorStatus = 1;
    system("clear");
    printf("Monitor Started Successfully.\n");
    if(close(pipefd[1]) == -1){
      printf("startMonitor MAIN close pipe write error.");
      return -1;
    }
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
    perror("listHunts :: Signal not sent :");
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
    perror("listTreasures2 :: Signal not sent :");
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
    perror("viewTreasure2 :: Signal not sent :");
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
    perror("stopMonitor :: Signal not sent :");
    return -1;
  }
  if(close(pipefd[0]) == -1){
    printf("stopMonitor MAIN close pipe read error.");
    return -1;
  }
  monitorStatus = 0;
  if(waitpid(monitorID, &status, 0) == -1){
    printf("stopMonitor waitpid failed.");
    return -1;
  }
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

  struct dirent *entry = {0};
  DIR *directory = opendir(".");

  if (directory == NULL) {
    perror("calculateScore :: OPENDIR ERROR :");
    return -1;
  }

  char directoryList[MAX][MAX] = {0};
  int directoryCounter = 0;
  char fullPath[3 * MAX] = {0};
  char command[3 * MAX] = {0};
  struct stat statbuf = {0};
  int processPipe[2] = {0};

  if (pipe(processPipe) == -1) {
    perror("calculateScore :: Pipe Creation Failed :");
    return -1;
  }

  while ((entry = readdir(directory))) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
      continue;

    if(snprintf(fullPath, sizeof(fullPath), "%s/%s", ".", entry->d_name) < 0){
      printf("calculateScore snprintf error");
      closedir(directory);
      return -1;
    }

    if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
      if (strcmp(entry->d_name, ".git") == 0) {
        continue;
      }
      strcpy(directoryList[directoryCounter++], entry->d_name);
    }
  }

  if(closedir(directory) == -1){
    printf("caculateScore closedir error.");
    return -1;
  }

  for (int i = 0; i < directoryCounter; i++) {
    pid_t pid = fork();
    if(pid < 0){
      printf("calculateScore :: side process did not start.");
      return -1;
    }
    if (pid == 0) {
      close(processPipe[0]);

      memset(command, 0, sizeof(command));
      if(snprintf(command, sizeof(command),"./calculate_score %s", directoryList[i]) == -1){
        printf("calculateScore :: failed to create command.");
        return -1;
      }

      if(dup2(processPipe[1], STDOUT_FILENO) == -1){
        printf("calculateScore :: failed to redirect STDOUT to pipe.");
        return -1;
      }
      if(close(processPipe[1]) == -1){
        printf("calculateScore :: failed to close processPipe write end.");
        return -1;
      }
      system(command);

      exit(0);
    }
  }

  for (int i = 0; i < directoryCounter; i++) {
    wait(NULL);
  }

  memset(buffer, 0, sizeof(buffer));
  if(read(processPipe[0], buffer, sizeof(buffer) - 1) == -1){
    printf("caculateScore :: failed to read from pipe.");
    return -1;
  }
  printf("%s", buffer);

  printf("\n");
  return 0;
}
