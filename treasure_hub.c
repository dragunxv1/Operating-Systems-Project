#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hub.h"

int option = 0;
COMMAND comm = 0;

int displayMenu() {
  while (1) {
    printf("Choose a command:\n");
    printf("1) Start Monitor.\n");
    printf("2) List Hunts.\n");
    printf("3) List Treasures.\n");
    printf("4) View Treasure.\n");
    printf("5) Stop Monitor.\n");
    printf("6) Calculate Score.\n");
    printf("0) EXIT.\n");

    if (scanf("%d", &option) != 1) {
      perror("Menu Scanf :");
      return -1;
    }
    getchar();
    comm = option;

    switch (comm) {
      case start_monitor:
        startMonitor();
        break;
      case list_hunts:
        listHunts();
        break;
      case list_treasures:
        listTreasures2();
        break;
      case view_treasures:
        viewTreasure2();
        break;
      case stop_monitor:
        stopMonitor();
        break;
      case calculate_score:
        calculateScore();
        break;
      case exit_program:
        if (closeProgram() == 0) {
          return 0;
        }
        break;
      default:
        system("clear");
        printf("Incorrect Command. Try again.\n");
    }
  }
}

int main(void) {
  displayMenu();

  return 0;
}