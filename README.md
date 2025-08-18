# Arduino-WiFi-Battleships
Repository for storing arduino code for running the Arduino WiFi Battleships project, and instructions for recreating the hardware.

## Setup
### Run the following commands for the arduino-cli:

```
    arduino-cli core update-index
    arduino-cli core install arduino:megaavr
    arduino-cli lib install "Adafruit NeoPixel"
    arduino-cli lib install "WiFiNINA"
```

## Running a sketch
### Use the following command to find your board's port and FQBN:

```
    arduino-cli board list
```

### Compile and upload the sketch to your board with:

```
    arduino-cli compile --fqbn <your boards FQBN here> <directory name containing .ino file>
    arduino-cli upload -p <your board's port here> --fqbn <your board's FQBN here> <directory name containing .ino file>
    
    arduino-cli compile --fqbn arduino:megaavr:uno2018 ./
    arduino-cli upload -p COM7 --fqbn arduino:megaavr:uno2018 ./
```

### For monitoring the board, use the command: 

```
    arduino-cli monitor -p <your board's port here> -b <your board's FQBN here>
    
    arduino-cli monitor -p COM7 -b arduino:megaavr:uno2018
```

TODO:
- after the above, basically everything has been covered and development can start.
- will need to do the following:
    - 4 stages - start screen, place ships, shoot ships, end screen
      - start screen: both players need to confirm that they want to start
      - place ships: need to allow for movement of each ship (possible have a cycle through ships button - but may not have the board space for this), rotation and placement
      - shoot ships: need to allow for movement and shooting - could allow for special shots (e.g., each player gets a single 2x2 and a full row/column shot per game)
      - end screen: simple green or red display depending on the outcome is probably fine here, can't really do anything complex with an 8x8 grid
    - communicate between boards for the actual game 
    - have server persistently store history of the matches, and a scoreboard, presented by a frontend

- write down description of hardware/components and their wiring
