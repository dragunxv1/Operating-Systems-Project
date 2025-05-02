#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "hub.h"

int main(void) {
  int x = 0;
  COMMAND comm = 0;

  while (1) {
    displayMenu();

    if (scanf("%d", &x) != 1) {
      perror("Menu Scanf :");
      return -1;
    }
    getchar();
    comm = x;

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

  return 0;
}