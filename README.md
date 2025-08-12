# Arduino-WiFi-Battleships
Repository for storing arduino code for running the Arduino WiFi Battleships project, and instructions for recreating the hardware.

TODO:
- work out how to use buttons and wire them through breadboard (may also need more breadboards for this, or some alternative)
- work out how to communicate between boards over WiFi

- after the above, basically everything has been covered and development can start.
- will need to do the following:
    - up, down, left, right movement
    - 4 stages - start screen, place ships, shoot ships, end screen
      - start screen: both players need to confirm that they want to start, maybe some kind of pairing is required too?
      - place ships: need to allow for movement of each ship (possible have a cycle through ships button - but may not have the board space for this), rotation and placement
      - shoot ships: need to allow for movement and shooting - could allow for special shots (e.g., each player gets a single 2x2 and a full row/column shot per game)
      - end screen: simple green or red display depending on the outcome is probably fine here, can't really do anything complex with an 8x8 grid

- write down description of hardware/compontents and their wiring
