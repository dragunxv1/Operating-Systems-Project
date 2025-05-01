

typedef enum {
  start_monitor = 1,
  list_hunts,
  list_treasures,
  view_treasures,
  stop_monitor,
  exit_program = 0
} COMMAND;

void displayMenu();
int listHunts();
int startMonitor();
int listTreasures();
int viewTreasure();
int stopMonitor();
int closeProgram();