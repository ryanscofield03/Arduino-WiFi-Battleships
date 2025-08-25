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
        int x;
        int y;

        void moveWithinBounds(int &pos, int direction) {
            pos += direction;
            pos = constrain(pos, 0, 7);
        };
    public:
        BoardPosition(int startX = 3, int startY = 4) {
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
            Serial.println("Drawing StartGameState");
            for (int i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(255, 19, 159));
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
    public:
        PlacingShipsState(GameContext& context) : GameState(context) {}

        void Draw() override {
            Serial.println("Drawing PlacingShipsState");
            for (int i = 0; i < NUMPIXELS; i++) {
                context.screen.setPixelColor(i, context.screen.Color(50, 100, 150));
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
            return StateType::PlaceShips;
        }
};

class ShootingShipsState: public GameState {
    private:
        BoardPosition hitPlacements[64];
        bool myTurn;
        int shootSize;
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
                delay(250);
                currentState->HandleLeft();
            } else if (upPressed) {
                Serial.println("Up pressed");
                delay(250);
                currentState->HandleUp();
            } else if (downPressed) {
                Serial.println("Down pressed");
                delay(250);
                currentState->HandleDown();
            } else if (rightPressed) {
                Serial.println("Right pressed");
                delay(250);
                currentState->HandleRight();
            } else if (rotatePressed) {
                Serial.println("Rotate pressed");
                delay(250);
                currentState->HandleRotate();
            } else if (actionPressed) {
                Serial.println("Action pressed");
                delay(250);
                currentState->HandleAction();
            }

            currentState->Draw();

            if (context.canTransitionFlag) {
                switch (currentState->Type()) {
                    case StateType::Start:
                        SetState(new PlacingShipsState(context));
                        context.canTransitionFlag = false;
                        break;
                }
            }

            delay(250);
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
