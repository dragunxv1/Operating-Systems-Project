#!/bin/bash
#This test program will create hunts and treasures to show how treasure_manager works and will
#delete everything when it's done

echo "Adding treasures to 3 different hunts"

ID=000
USER="Indiana Jones"
VALUE=1000
CLUE="Under the old oak"
X=45.67
Y=12.34

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game10 > /dev/null


ID=001
USER="Lara Croft"
VALUE=2500
CLUE="Behind the ancient statue"
X=23.45
Y=87.12

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game10 > /dev/null


ID=002
USER="Nathan Drake"
VALUE=1900
CLUE="Buried under the palm tree"
X=12.67
Y=45.89

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game10 > /dev/null

ID=003
USER="Evelyn Salt"
VALUE=3000
CLUE="Inside the waterfall cave"
X=9.81
Y=33.22

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game10 > /dev/null


ID=100
USER="Ben Gates"
VALUE=2750
CLUE="Under the Liberty Bell"
X=38.65
Y=77.03

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game11 > /dev/null

ID=101
USER="Rick Connel"
VALUE=3200
CLUE="Inside the pyramid tomb"
X=29.99
Y=31.21

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game11 > /dev/null

ID=102
USER="Allan Quatermain"
VALUE=2100
CLUE="Near the stone totem"
X=41.30
Y=80.55

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game11 > /dev/null

ID=200
USER="Nancy Drew"
VALUE=1800
CLUE="Left of the old chapel"
X=60.12
Y=22.88

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game12 > /dev/null

ID=201
USER="Sherlock Holmes"
VALUE=2900
CLUE="Inside the hollow tree"
X=451.50
Y=800.12

printf "%d\n%s\n%d\n%s\n%.2f\n%.2f\n" \
  "$ID" "$USER" "$VALUE" "$CLUE" "$X" "$Y" | ./treasure_manager --add game12 > /dev/null


echo "________________________________________________________________________________"
echo "LISTING HUNTS"
echo "TRYING TO LIST A HUNT THAT DOESN'T EXIST"
echo "________________________________________________________________________________"
./treasure_manager --list game10
echo "________________________________________________________________________________"
./treasure_manager --list game11
echo "________________________________________________________________________________"
./treasure_manager --list game12
echo "________________________________________________________________________________"
./treasure_manager --list game15


echo "________________________________________________________________________________"
echo "LISTING A SPECIFIC TREASURE FROM EACH HUNT"
echo "TRYING TO LIST A TREASURE THAT DOESN'T' EXIST"
echo "TRYING TO LIST A TREASURE FROM A HUNT THAT DOESN'T EXIST"
echo "________________________________________________________________________________"
./treasure_manager --view game10 000
echo "________________________________________________________________________________"
./treasure_manager --view game11 101
echo "________________________________________________________________________________"
./treasure_manager --view game12 201
echo "________________________________________________________________________________"
./treasure_manager --view game12 205
echo "________________________________________________________________________________"
./treasure_manager --view game15 202

echo "________________________________________________________________________________"
echo "DELETING 2 SPECIFIC TREASURES FROM EACH HUNT AND LISTING HUNTS"
echo "________________________________________________________________________________"
./treasure_manager --remove_treasure game10 000
./treasure_manager --remove_treasure game10 001
./treasure_manager --remove_treasure game11 102
./treasure_manager --remove_treasure game11 101
./treasure_manager --remove_treasure game12 200
./treasure_manager --remove_treasure game12 201

./treasure_manager --list game10
echo "________________________________________________________________________________"
./treasure_manager --list game11
echo "________________________________________________________________________________"
./treasure_manager --list game12

echo "________________________________________________________________________________"
echo "LISTING HUNTS"
echo "________________________________________________________________________________"
ls -d */

echo "________________________________________________________________________________"
echo "DELETING 2 HUNTS AND LISTING HUNTS"
echo "________________________________________________________________________________"
#./treasure_manager --remove_hunt game10
#./treasure_manager --remove_hunt game12
ls -d */
#./treasure_manager --remove_hunt game11


