#include "hub.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

pid_t monitorID = -1;
int monitorStatus = 0;

void displayMenu() {
  printf("Choose a command:\n");
  printf("1) Start Monitor.\n");
  printf("2) List Hunts.\n");
  printf("3) List Treasures.\n");
  printf("4) View Treasure.\n");
  printf("5) Stop Monitor.\n");
  printf("0) EXIT.\n");
}

void signalHandler(int signal) {
  if (signal == SIGUSR1) {
    system("clear");
    printf("LIST HUNTS\n");
    displayMenu();
  } else if (signal == SIGUSR2) {
    system("clear");
    printf("LIST TREASURES\n");
    displayMenu();
  } else if (signal == SIGINT) {
    system("clear");
    printf("VIEW TREASURE DETAILS\n");
    displayMenu();
  } else if (signal == SIGTERM) {
    system("clear");
    printf("Terminating Monitor.\n");
    _exit(0);
  }
}

int startMonitor() {
  if (monitorID > 0) {
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

int listTreasures() {
  if (monitorStatus == 0) {
    printf("Monitor not running.\n");
    return -1;
  }
  if (kill(monitorID, SIGUSR2) == -1) {
    perror("Signal not sent :");
    return -1;
  }
  return 0;
}

int viewTreasure() {
  if (monitorStatus == 0) {
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
