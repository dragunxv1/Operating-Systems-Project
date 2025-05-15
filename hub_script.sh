gcc -Wall -o treasure_hub hub.c manager.c treasure_hub.c
valgrind --leak-check=full --show-leak-kinds=all ./treasure_hub 