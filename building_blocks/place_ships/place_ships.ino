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

enum Direction {
    LEFT, RIGHT, UP, DOWN
};

struct BoardPosition {
    private:
        int startX, startY;
        int x;
        int y;

        void moveWithinBounds(int &pos, int direction) {
            pos += direction;
            pos = constrain(pos, 0, 7);
        };
    public:
        BoardPosition(int startX = 3, int startY = 4) {
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

        int to_index() {
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
            for (int i = 0; i < NUMPIXELS; i++) {
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
        int currentShipIndex;
        int rotation;
        bool currentShip[3][3];

    public:
        PlacingShipsState(GameContext& context) : GameState(context) {
            currentShipIndex = 0;
            rotation = 0;
            UpdateCurrentShip();
        }

        void UpdateCurrentShip() {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    currentShip[i][j] = ships[currentShipIndex][rotation][i][j];
                }
            }
        }

        void DrawRelativeShip(bool ship[3][3], int shipX, int shipY, int r, int g, int b) {
            for (int i = 0; i < 3; i++) {
                for (int j = 0; j < 3; j++) {
                    if (ship[j][i]) {
                        int relativeX = shipX + i - 1;
                        int relativeY = shipY + j - 1;

                        if (relativeX >= 0 && relativeX < 8 && relativeY >= 0 && relativeY < 8) {
                            int pixelIndex = relativeY * 8 + relativeX;
                            context.screen.setPixelColor(pixelIndex, context.screen.Color(r, g, b));
                        }
                    }
                }
            }
        }

        void Draw() override {
            for (int i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(0, 0, 0));
            }

            for (int s = 0; s < 4; s++) {
                int shipX = context.shipPlacements[s].to_index() % 8;
                int shipY = context.shipPlacements[s].to_index() / 8;
                DrawRelativeShip(context.selectedShips[s], shipX, shipY, 20, 20, 255);
            }

            int posX = context.position.to_index() % 8;
            int posY = context.position.to_index() / 8;
            DrawRelativeShip(currentShip, posX, posY, 255, 255, 255);

            // TODO - make pixel RED if there is overlap (e.g., does not allow placement)

            context.screen.show();
        }

        void HandleRotate() override {
            rotation = (rotation + 1) % 4;
            UpdateCurrentShip();
        }

        void HandleAction() override {
            if (currentShipIndex < 3) {
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 3; j++) {
                        context.selectedShips[currentShipIndex][i][j] = currentShip[i][j];
                    }
                }
                context.shipPlacements[currentShipIndex] = context.position;
                currentShipIndex++;
                UpdateCurrentShip();
                context.position.to_center();
            } else {
                context.canTransitionFlag = true;
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
        int shootSize;
    public:
        ShootingShipsState(GameContext& context) : GameState(context) {}

        void Draw() override {
            for (int i = 0; i < NUMPIXELS; i++) {
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
            int upPressed = digitalRead(UP_PIN) == LOW;
            int downPressed = digitalRead(DOWN_PIN) == LOW;
            int rightPressed = digitalRead(RIGHT_PIN) == LOW;
            int rotatePressed = digitalRead(ROTATE_PIN) == LOW;
            int actionPressed = digitalRead(ACTION_PIN) == LOW;

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
