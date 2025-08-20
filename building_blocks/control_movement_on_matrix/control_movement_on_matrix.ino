#include <Adafruit_NeoPixel.h>

#define LEFT_PIN 7
#define UP_PIN 6
#define DOWN_PIN 5
#define RIGHT_PIN 4
#define ROTATE_PIN 3 // both rotate your shooting method (e.g., 1x1, or your single 2x2 shot), and rotating ships when placing them
#define ACTION_PIN 2

#define LED_MATRIX_PIN 8
#define NUMPIXELS 64

Adafruit_NeoPixel strip(NUMPIXELS, LED_MATRIX_PIN, NEO_GRB + NEO_KHZ800);

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

BoardPosition playerPosition;

void setup() {
    pinMode(LEFT_PIN, INPUT_PULLUP);
    pinMode(UP_PIN, INPUT_PULLUP);
    pinMode(DOWN_PIN, INPUT_PULLUP);
    pinMode(RIGHT_PIN, INPUT_PULLUP);
    pinMode(ROTATE_PIN, INPUT_PULLUP);
    pinMode(ACTION_PIN, INPUT_PULLUP);

    strip.begin();
    strip.setBrightness(2);
    strip.show();

    Serial.begin(9600);
    delay(1000);
}


void loop() {
    bool leftPressed = digitalRead(LEFT_PIN) == LOW;
    int upPressed = digitalRead(UP_PIN) == LOW;
    int downPressed = digitalRead(DOWN_PIN) == LOW;
    int rightPressed = digitalRead(RIGHT_PIN) == LOW;
    int rotatePressed = digitalRead(ROTATE_PIN) == LOW;
    int actionPressed = digitalRead(ACTION_PIN) == LOW;

    if (leftPressed) {
        Serial.println("Left pressed");
        playerPosition.move(LEFT);
        playerPosition.print();
        delay(250);
    } else if (upPressed) {
        Serial.println("Up pressed");
        playerPosition.move(UP);
        playerPosition.print();
        delay(250);
    } else if (downPressed) {
        Serial.println("Down pressed");
        playerPosition.move(DOWN);
        playerPosition.print();
        delay(250);
    } else if (rightPressed) {
        Serial.println("Right pressed");
        playerPosition.move(RIGHT);
        playerPosition.print();
        delay(250);
    } else if (rotatePressed) {
        Serial.println("Rotate pressed");
        delay(250);
    } else if (actionPressed) {
        Serial.println("Action pressed");
        delay(250);
    }

    // TODO: we will have a 1 second delay when redrawing the board currently
    // - need to delay after drawing but only if any button was pressed

    int playerIndex = playerPosition.to_index();
    for (int i = 0; i < NUMPIXELS; i++) {
        if (i == playerIndex) {
            strip.setPixelColor(i, strip.Color(255, 19, 159));
        } else {
            strip.setPixelColor(i, strip.Color(0, 0, 0));
        }
    }
    strip.show();
    delay(100);
}
