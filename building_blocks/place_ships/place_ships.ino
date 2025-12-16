#include <Adafruit_NeoPixel.h>

#define LEFT_PIN 7
#define UP_PIN 6
#define DOWN_PIN 5
#define RIGHT_PIN 4
#define ROTATE_PIN 3 // both rotate your shooting method (e.g., 1x1, or your single 2x2 shot), and rotating ships when placing them
#define ACTION_PIN 2

#define LED_MATRIX_PIN 8
#define NUMPIXELS 64

bool ships[4][4][3][3] = {
    {
        {   // small L ship 0 degrees
            {false, true, false},
            {false, true, true},
            {false, false, false}
        },
        {   // small L ship 90 degrees
            {false, false, false},
            {false, true, true},
            {false, true, false}
        },
        {   // small L ship 180 degrees
            {false, false, false},
            {true, true, false},
            {false, true, false}
        },
        {   // small L ship 270 degrees
            {false, true, false},
            {true, true, false},
            {false, false, false}
        }
    },
    {
        {   // big L ship 0 degrees
            {false, true, false},
            {false, true, false},
            {false, true, true}
        },
        {   // big L ship 90 degrees
            {false, false, false},
            {true, true, true},
            {false, false, true}
        },
        {   // big L ship 180 degrees
            {false, true, false},
            {false, true, false},
            {true, true, false}
        },
        {   // big L ship 270 degrees
            {true, false, false},
            {true, true, true},
            {false, false, false}
        }
    },
    {
        {   // 3x1 ship 0 degrees
            {false, true, false},
            {false, true, false},
            {false, false, false}
        },
        {   // 3x1 ship 90 degrees
            {false, false, false},
            {false, true, true},
            {false, false, false}
        },
        {   // 3x1 ship 180 degrees
            {false, false, false},
            {false, true, false},
            {false, true, false}
        },
        {   // 3x1 ship 270 degrees
            {false, false, false},
            {true, true, false},
            {false, false, false}
        }
    },
    {
        {   // 3x1 ship 0 degrees
            {false, true, false},
            {false, true, false},
            {false, true, false}
        },
        {   // 3x1 ship 90 degrees
            {false, false, false},
            {true, true, true},
            {false, false, false}
        },
        {   // 3x1 ship 180 degrees
            {false, true, false},
            {false, true, false},
            {false, true, false}
        },
        {   // 3x1 ship 270 degrees
            {false, false, false},
            {true, true, true},
            {false, false, false}
        }
    }
};

struct Coordinate {
    public:
        uint8_t x;
        uint8_t y;
};

enum Direction {
    LEFT, RIGHT, UP, DOWN
};

struct BoardPosition {
    private:
        uint8_t startX, startY;
        uint8_t x;
        uint8_t y;

        void moveWithinBounds(uint8_t &pos, uint8_t direction) {
            pos += direction;
            pos = constrain(pos, 0, 7);
        };
    public:
        BoardPosition(uint8_t startX = 3, uint8_t startY = 4) {
            this->startX = startX;
            this->startY = startY;
            x = startX;
            y = startY;
        };

        void move(Direction direction) {
            if (direction == LEFT) {
                moveWithinBounds(x, -1);
            } else if (direction == RIGHT) {
                moveWithinBounds(x, 1);
            } else if (direction == UP) {
                moveWithinBounds(y, -1);
            } else if (direction == DOWN) {
                moveWithinBounds(y, 1);
            }
        };

        void to_center() {
            x = startX;
            y = startY;
        }

        void print() {
            Serial.print("{ x: ");
            Serial.print(x);
            Serial.print(", y: ");
            Serial.print(y);
            Serial.println(" }");
        };

        uint8_t to_index() {
            return (8 * y) + x;
        };
};

enum class StateType {
    Start,
    PlaceShips,
    ShootShips,
    End
};

struct GameContext {
    bool canTransitionFlag = false;
    BoardPosition position;
    BoardPosition shipPlacements[4];
	bool selectedShips[4][3][3];
    Adafruit_NeoPixel& screen;

    GameContext(Adafruit_NeoPixel& screen) : screen(screen) {}
};

class GameState {
    protected:
        GameContext& context;
    public:
        GameState(GameContext& context) : context(context) {}
        virtual ~GameState() {}
        virtual void Draw() = 0;

        virtual void HandleLeft() {
            context.position.move(LEFT);
        };

        virtual void HandleRight() {
            context.position.move(RIGHT);
        };

        virtual void HandleUp() {
            context.position.move(UP);
        };

        virtual void HandleDown() {
            context.position.move(DOWN);
        };

        virtual void HandleRotate() = 0;
        virtual void HandleAction() = 0;
        virtual StateType Type() = 0;
};

class StartGameState: public GameState {
    public:
        StartGameState(GameContext& context) : GameState(context) {}

        void Draw() override {
            for (uint8_t i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(20, 20, 20, 1));
            }
            context.screen.show();
        }

        void HandleRotate() override {
            ;
        }

        void HandleAction() override {
            context.canTransitionFlag = true;
        }

        StateType Type() override {
            return StateType::Start;
        }
};

class PlacingShipsState: public GameState {
    private:
        uint8_t currentShipIndex;
        uint8_t rotation;
        bool currentShip[3][3];

    public:
        PlacingShipsState(GameContext& context) : GameState(context) {
            currentShipIndex = 0;
            rotation = 0;
            UpdateCurrentShip();
        }

        void UpdateCurrentShip() {
            for (uint8_t y = 0; y < 3; y++) {
                for (uint8_t x = 0; x < 3; x++) {
                    currentShip[y][x] = ships[currentShipIndex][rotation][y][x];
                }
            }
        }

        void DrawRelativeShip(bool ship[3][3], BoardPosition position, uint8_t r, uint8_t g, uint8_t b) {
            uint8_t shipX = position.to_index() % 8;
            uint8_t shipY = position.to_index() / 8; 
            for (uint8_t y = 0; y < 3; y++) {
                for (uint8_t x = 0; x < 3; x++) {
                    if (ship[y][x]) {
                        uint8_t px = shipX + x - 1;
                        uint8_t py = shipY + y - 1;

                        if (px >= 0 && px < 8 && py >= 0 && py < 8) {
                            uint8_t pixelIndex = py * 8 + px;
                            context.screen.setPixelColor(pixelIndex, context.screen.Color(r, g, b));
                        }
                    }
                }
            }
        }

        uint8_t GetCoordinatesFromPosition(bool ship[3][3], BoardPosition position, Coordinate outCoords[9]) {
            uint8_t x = position.to_index() % 8;
            uint8_t y = position.to_index() / 8;
            uint8_t count = 0;
            for (uint8_t y = 0; y < 3; y++) {
                for (uint8_t x = 0; x < 3; x++) {
                    if (ship[y][x]) {
                        outCoords[count++] = {shipX + x - 1, shipY + y - 1};
                    }
                }
            }
            return count;
        }

        void Draw() override {
            // Clear the screen
            for (uint8_t i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(0, 0, 0));
            }

            // Draw currently placed ships
            bool occupied[8][8] = {};
            for (uint8_t s = 0; s < 4; s++) {
                // Build up a list of coordinates that are already used
                Coordinate shipCoords[9];
                uint8_t count = GetCoordinatesFromPosition(
                    context.selectedShips[s], 
                    context.shipPlacements[s],
                    shipCoords
                );
                for (uint8_t i = 0; i < count; i++) {
                    if (shipCoords[i].x >= 0 && shipCoords[i].x < 8 && shipCoords[i].y >= 0 && shipCoords[i].y < 8)
                        occupied[shipCoords[i].y][shipCoords[i].x] = true;
                }

                DrawRelativeShip(
                    context.selectedShips[s], 
                    context.shipPlacements[s], 
                    20, 
                    20, 
                    255
                );
            }

            // Build a list of current coordinates
            Coordinate currentCoords[9];
            uint8_t currentCount = GetCoordinatesFromPosition(
                currentShip, 
                context.position,
                currentCoords
            );

            // Determine if there is overlap between current ship and placed ships
            bool overlap = false;
            for (uint8_t i = 0; i < currentCount; i++) {
                if (currentCoords[i].x >= 0 && currentCoords[i].x < 8 &&
                    currentCoords[i].y >= 0 && currentCoords[i].y < 8 &&
                    occupied[currentCoords[i].y][currentCoords[i].x]) {
                    overlap = true;
                    break;
                }
            }

            // Draw current ship (white if no overlap, red if overlap)
            DrawRelativeShip(
                currentShip, 
                context.position, 
                255, 
                overlap ? 0 : 255, 
                overlap ? 0 : 255
            );

            context.screen.show();
        }

        void HandleRotate() override {
            rotation = (rotation + 1) % 4;
            UpdateCurrentShip();
        }

        void HandleAction() override {
            if (currentShipIndex < 4) {
                // Check that the entire ship is inbounds and not overlapping with another ship

                for (uint8_t y = 0; y < 3; y++) {
                    for (uint8_t x = 0; x < 3; x++) {
                        context.selectedShips[currentShipIndex][y][x] = currentShip[y][x];
                    }
                }
                context.shipPlacements[currentShipIndex] = context.position;
                currentShipIndex++;

                if (currentShipIndex < 4) {
                    UpdateCurrentShip();
                    context.position.to_center();
                } else {
                    context.canTransitionFlag = true;
                }
            }
        }

        StateType Type() override {
            return StateType::PlaceShips;
        }
};

class ShootingShipsState: public GameState {
    private:
        BoardPosition hitPlacements[64];
        bool myTurn;
        uint8_t shootSize;
    public:
        ShootingShipsState(GameContext& context) : GameState(context) {}

        void Draw() override {
            for (uint8_t i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(20, 20, 20, 1));
            }
            context.screen.show();
        }

        void HandleRotate() override {
            ;
        }

        void HandleAction() override {
            ;
        }

        StateType Type() override {
            return StateType::ShootShips;
        }
};

class Game {
    private:
        GameState* currentState;
        GameContext& context;

    public:
        Game(GameContext& context) : currentState(nullptr), context(context) {}

        void SetState(GameState* newState) {
            if (currentState) delete currentState;
            currentState = newState;
        }

        void Update() {
            bool leftPressed = digitalRead(LEFT_PIN) == LOW;
            uint8_t upPressed = digitalRead(UP_PIN) == LOW;
            uint8_t downPressed = digitalRead(DOWN_PIN) == LOW;
            uint8_t rightPressed = digitalRead(RIGHT_PIN) == LOW;
            uint8_t rotatePressed = digitalRead(ROTATE_PIN) == LOW;
            uint8_t actionPressed = digitalRead(ACTION_PIN) == LOW;

            if (leftPressed) {
                Serial.println("Left pressed");
                currentState->HandleLeft();
            } else if (upPressed) {
                Serial.println("Up pressed");
                currentState->HandleUp();
            } else if (downPressed) {
                Serial.println("Down pressed");
                currentState->HandleDown();
            } else if (rightPressed) {
                Serial.println("Right pressed");
                currentState->HandleRight();
            } else if (rotatePressed) {
                Serial.println("Rotate pressed");
                currentState->HandleRotate();
            } else if (actionPressed) {
                Serial.println("Action pressed");
                currentState->HandleAction();
            }

            currentState->Draw();

            if (context.canTransitionFlag) {
                switch (currentState->Type()) {
                    case StateType::Start:
                        SetState(new PlacingShipsState(context));
                        context.canTransitionFlag = false;
                        break;
                    case StateType::PlaceShips:
                        SetState(new ShootingShipsState(context));
                        context.canTransitionFlag = false;
                        break;
                }
            }

            delay(100);
        }

        ~Game() {
            delete currentState;
        }
};

Adafruit_NeoPixel screen(NUMPIXELS, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);
GameContext context(screen);
Game game(context);

void setup() {
    pinMode(LEFT_PIN, INPUT_PULLUP);
    pinMode(UP_PIN, INPUT_PULLUP);
    pinMode(DOWN_PIN, INPUT_PULLUP);
    pinMode(RIGHT_PIN, INPUT_PULLUP);
    pinMode(ROTATE_PIN, INPUT_PULLUP);
    pinMode(ACTION_PIN, INPUT_PULLUP);

    screen.begin();
    screen.setBrightness(2);
    screen.show();

    Serial.begin(9600);
    delay(1000);

    game.SetState(new StartGameState(context));
}


void loop() {
    game.Update();
    delay(100);
}
